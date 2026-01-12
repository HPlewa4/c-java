#include "model/conv_layer.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstring>

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

Conv2DLayer::Conv2DLayer(int in_ch, int out_ch, int k_size, int stride, int pad)
    : in_channels(in_ch), out_channels(out_ch), kernel_size(k_size),
      stride(stride), padding(pad), batch_accumulation_count(0),
      last_output_height(0), last_output_width(0),
      rng(std::random_device{}()) {

    double std_dev = std::sqrt(2.0 / (in_channels * kernel_size * kernel_size));
    std::normal_distribution<double> dist(0.0, std_dev);

    weights.resize(out_channels);
    weight_grad_accum.resize(out_channels);
    for (int oc = 0; oc < out_channels; oc++) {
        weights[oc].resize(in_channels);
        weight_grad_accum[oc].resize(in_channels);
        for (int ic = 0; ic < in_channels; ic++) {
            weights[oc][ic].resize(kernel_size);
            weight_grad_accum[oc][ic].resize(kernel_size);
            for (int kh = 0; kh < kernel_size; kh++) {
                weights[oc][ic][kh].resize(kernel_size);
                weight_grad_accum[oc][ic][kh].resize(kernel_size, 0.0);
                for (int kw = 0; kw < kernel_size; kw++) {
                    weights[oc][ic][kh][kw] = dist(rng);
                }
            }
        }
    }

    biases.resize(out_channels, 0.0);
    bias_grad_accum.resize(out_channels, 0.0);
}

std::vector<std::vector<std::vector<double>>> Conv2DLayer::apply_padding(
    const std::vector<std::vector<std::vector<double>>>& input) const {

    if (padding == 0) {
        return input;
    }

    int channels = input.size();
    int height = input[0].size();
    int width = input[0][0].size();

    int new_height = height + 2 * padding;
    int new_width = width + 2 * padding;

    std::vector<std::vector<std::vector<double>>> padded(
        channels,
        std::vector<std::vector<double>>(
            new_height,
            std::vector<double>(new_width, 0.0)));

    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                padded[c][h + padding][w + padding] = input[c][h][w];
            }
        }
    }

    return padded;
}

std::vector<double> Conv2DLayer::im2col(
    const std::vector<std::vector<std::vector<double>>>& input,
    int output_height, int output_width) const {

    int input_height = input[0].size();
    int input_width = input[0][0].size();

    int col_height = in_channels * kernel_size * kernel_size;
    int col_width = output_height * output_width;

    std::vector<double> col(col_height * col_width);

    int col_idx = 0;
    for (int h_out = 0; h_out < output_height; h_out++) {
        for (int w_out = 0; w_out < output_width; w_out++) {
            for (int ic = 0; ic < in_channels; ic++) {
                for (int kh = 0; kh < kernel_size; kh++) {
                    for (int kw = 0; kw < kernel_size; kw++) {
                        int h_in = h_out * stride + kh;
                        int w_in = w_out * stride + kw;

                        int row = ic * kernel_size * kernel_size + kh * kernel_size + kw;
                        col[row * col_width + col_idx] = input[ic][h_in][w_in];
                    }
                }
            }
            col_idx++;
        }
    }
    return col;
}

std::vector<std::vector<std::vector<double>>> Conv2DLayer::col2im(
    const std::vector<double>& col,
    int input_channels, int input_height, int input_width,
    int output_height, int output_width) const {

    std::vector<std::vector<std::vector<double>>> result(
        input_channels,
        std::vector<std::vector<double>>(
            input_height,
            std::vector<double>(input_width, 0.0)));

    int col_width = output_height * output_width;
    int col_idx = 0;

    for (int h_out = 0; h_out < output_height; h_out++) {
        for (int w_out = 0; w_out < output_width; w_out++) {
            for (int ic = 0; ic < input_channels; ic++) {
                for (int kh = 0; kh < kernel_size; kh++) {
                    for (int kw = 0; kw < kernel_size; kw++) {
                        int h_in = h_out * stride + kh;
                        int w_in = w_out * stride + kw;

                        int row = ic * kernel_size * kernel_size + kh * kernel_size + kw;
                        result[ic][h_in][w_in] += col[row * col_width + col_idx];
                    }
                }
            }
            col_idx++;
        }
    }
    return result;
}

std::vector<std::vector<std::vector<double>>> Conv2DLayer::forward(
    const std::vector<std::vector<std::vector<double>>>& input) {

    if (input.empty() || input[0].empty() || input[0][0].empty()) {
        throw std::invalid_argument("Conv2DLayer: empty input");
    }

    if ((int)input.size() != in_channels) {
        throw std::invalid_argument("Conv2DLayer: input channels mismatch");
    }

    last_input = input;

    auto padded_input = apply_padding(input);

    int input_height = padded_input[0].size();
    int input_width = padded_input[0][0].size();

    last_output_height = (input_height - kernel_size) / stride + 1;
    last_output_width = (input_width - kernel_size) / stride + 1;

    im2col_data = im2col(padded_input, last_output_height, last_output_width);

    int weight_rows = out_channels;
    int weight_cols = in_channels * kernel_size * kernel_size;
    std::vector<double> weight_matrix(weight_rows * weight_cols);

    for (int oc = 0; oc < out_channels; oc++) {
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    int col = ic * kernel_size * kernel_size + kh * kernel_size + kw;
                    weight_matrix[oc * weight_cols + col] = weights[oc][ic][kh][kw];
                }
            }
        }
    }

    // Matrix multiplication using BLAS: Y = alpha * A * B + beta * C
    // A: weight_matrix [out_channels x weight_cols]
    // B: im2col_data [weight_cols x (output_height * output_width)]
    // Y: output_matrix [out_channels x (output_height * output_width)]

    int m = out_channels;
    int n = last_output_height * last_output_width;
    int k = weight_cols;

    std::vector<double> output_matrix(m * n, 0.0);

#ifdef __APPLE__
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                m, n, k,
                1.0, weight_matrix.data(), k,
                im2col_data.data(), n,
                0.0, output_matrix.data(), n);
#else
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int p = 0; p < k; p++) {
                sum += weight_matrix[i * k + p] * im2col_data[p * n + j];
            }
            output_matrix[i * n + j] = sum;
        }
    }
#endif

    std::vector<std::vector<std::vector<double>>> output(
        out_channels,
        std::vector<std::vector<double>>(
            last_output_height,
            std::vector<double>(last_output_width)));

    for (int oc = 0; oc < out_channels; oc++) {
        for (int h = 0; h < last_output_height; h++) {
            for (int w = 0; w < last_output_width; w++) {
                int idx = h * last_output_width + w;
                output[oc][h][w] = output_matrix[oc * n + idx] + biases[oc];
            }
        }
    }
    return output;
}

std::vector<std::vector<std::vector<double>>> Conv2DLayer::backward(
    const std::vector<std::vector<std::vector<double>>>& output_grad) {

    if (last_input.empty()) {
        throw std::runtime_error("Conv2DLayer: backward called before forward");
    }

    auto padded_input = apply_padding(last_input);

    int input_height = last_input[0].size();
    int input_width = last_input[0][0].size();
    int padded_height = padded_input[0].size();
    int padded_width = padded_input[0][0].size();

    int m = out_channels;
    int n = last_output_height * last_output_width;
    std::vector<double> output_grad_matrix(m * n);

    for (int oc = 0; oc < out_channels; oc++) {
        for (int h = 0; h < last_output_height; h++) {
            for (int w = 0; w < last_output_width; w++) {
                int idx = h * last_output_width + w;
                output_grad_matrix[oc * n + idx] = output_grad[oc][h][w];

                bias_grad_accum[oc] += output_grad[oc][h][w];
            }
        }
    }

    // Compute weight gradient: dW = dY * X^T
    // dY: [out_channels x n]
    // X^T: [n x weight_cols] (im2col_data transposed)
    // dW: [out_channels x weight_cols]

    int weight_cols = in_channels * kernel_size * kernel_size;
    std::vector<double> weight_grad_matrix(m * weight_cols, 0.0);

#ifdef __APPLE__
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans,
                m, weight_cols, n,
                1.0, output_grad_matrix.data(), n,
                im2col_data.data(), n,
                0.0, weight_grad_matrix.data(), weight_cols);
#else
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < weight_cols; j++) {
            double sum = 0.0;
            for (int p = 0; p < n; p++) {
                sum += output_grad_matrix[i * n + p] * im2col_data[j * n + p];
            }
            weight_grad_matrix[i * weight_cols + j] = sum;
        }
    }
#endif
    for (int oc = 0; oc < out_channels; oc++) {
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    int col = ic * kernel_size * kernel_size + kh * kernel_size + kw;
                    weight_grad_accum[oc][ic][kh][kw] += weight_grad_matrix[oc * weight_cols + col];
                }
            }
        }
    }

    // Compute input gradient: dX_col = W^T * dY
    // W^T: [weight_cols x out_channels]
    // dY: [out_channels x n]
    // dX_col: [weight_cols x n]

    std::vector<double> input_grad_col(weight_cols * n, 0.0);

    std::vector<double> weight_matrix(m * weight_cols);
    for (int oc = 0; oc < out_channels; oc++) {
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    int col = ic * kernel_size * kernel_size + kh * kernel_size + kw;
                    weight_matrix[oc * weight_cols + col] = weights[oc][ic][kh][kw];
                }
            }
        }
    }

#ifdef __APPLE__
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans,
                weight_cols, n, m,
                1.0, weight_matrix.data(), weight_cols,
                output_grad_matrix.data(), n,
                0.0, input_grad_col.data(), n);
#else
    for (int i = 0; i < weight_cols; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int p = 0; p < m; p++) {
                sum += weight_matrix[p * weight_cols + i] * output_grad_matrix[p * n + j];
            }
            input_grad_col[i * n + j] = sum;
        }
    }
#endif

    auto padded_input_grad = col2im(input_grad_col, in_channels, padded_height, padded_width,
                                    last_output_height, last_output_width);

    std::vector<std::vector<std::vector<double>>> input_grad(
        in_channels,
        std::vector<std::vector<double>>(
            input_height,
            std::vector<double>(input_width, 0.0)));

    for (int ic = 0; ic < in_channels; ic++) {
        for (int h = 0; h < input_height; h++) {
            for (int w = 0; w < input_width; w++) {
                input_grad[ic][h][w] = padded_input_grad[ic][h + padding][w + padding];
            }
        }
    }

    batch_accumulation_count++;
    return input_grad;
}

void Conv2DLayer::zero_gradients() {
    for (int oc = 0; oc < out_channels; oc++) {
        bias_grad_accum[oc] = 0.0;
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    weight_grad_accum[oc][ic][kh][kw] = 0.0;
                }
            }
        }
    }
    batch_accumulation_count = 0;
}

void Conv2DLayer::apply_gradients() {
    if (batch_accumulation_count == 0) {
        return;
    }

    double scale = 1.0 / batch_accumulation_count;

    for (int oc = 0; oc < out_channels; oc++) {
        bias_grad_accum[oc] *= scale;
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    weight_grad_accum[oc][ic][kh][kw] *= scale;
                }
            }
        }
    }

    if (weight_optimizer) {
        weight_optimizer->update_4d(weights, weight_grad_accum);
    }
    if (bias_optimizer) {
        bias_optimizer->update_1d(biases, bias_grad_accum);
    }

    zero_gradients();
}

void Conv2DLayer::set_optimizer(OptimizerType opt_type, double learning_rate) {
    weight_optimizer = OptimizerFactory::create(opt_type, learning_rate);
    bias_optimizer = OptimizerFactory::create(opt_type, learning_rate);
}

void Conv2DLayer::save(std::ofstream& file) const {
    file.write(reinterpret_cast<const char*>(&in_channels), sizeof(int));
    file.write(reinterpret_cast<const char*>(&out_channels), sizeof(int));
    file.write(reinterpret_cast<const char*>(&kernel_size), sizeof(int));
    file.write(reinterpret_cast<const char*>(&stride), sizeof(int));
    file.write(reinterpret_cast<const char*>(&padding), sizeof(int));

    for (int oc = 0; oc < out_channels; oc++) {
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    file.write(reinterpret_cast<const char*>(&weights[oc][ic][kh][kw]), sizeof(double));
                }
            }
        }
    }
    for (int oc = 0; oc < out_channels; oc++) {
        file.write(reinterpret_cast<const char*>(&biases[oc]), sizeof(double));
    }
}

void Conv2DLayer::load(std::ifstream& file) {
    int loaded_in_ch, loaded_out_ch, loaded_k_size, loaded_stride, loaded_pad;
    file.read(reinterpret_cast<char*>(&loaded_in_ch), sizeof(int));
    file.read(reinterpret_cast<char*>(&loaded_out_ch), sizeof(int));
    file.read(reinterpret_cast<char*>(&loaded_k_size), sizeof(int));
    file.read(reinterpret_cast<char*>(&loaded_stride), sizeof(int));
    file.read(reinterpret_cast<char*>(&loaded_pad), sizeof(int));

    if (loaded_in_ch != in_channels || loaded_out_ch != out_channels ||
        loaded_k_size != kernel_size || loaded_stride != stride || loaded_pad != padding) {
        throw std::runtime_error("Conv2DLayer: saved layer configuration doesn't match");
    }

    for (int oc = 0; oc < out_channels; oc++) {
        for (int ic = 0; ic < in_channels; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    file.read(reinterpret_cast<char*>(&weights[oc][ic][kh][kw]), sizeof(double));
                }
            }
        }
    }

    for (int oc = 0; oc < out_channels; oc++) {
        file.read(reinterpret_cast<char*>(&biases[oc]), sizeof(double));
    }
}