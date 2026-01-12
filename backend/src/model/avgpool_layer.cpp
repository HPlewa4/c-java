#include "../../include/model/avgpool_layer.h"
#include <stdexcept>
#include <iostream>

AvgPool2DLayer::AvgPool2DLayer(int pool_size, int stride)
    : pool_size(pool_size),
      stride(stride == -1 ? pool_size : stride),
      input_channels(0), input_height(0), input_width(0) {
}

std::vector<std::vector<std::vector<double>>> AvgPool2DLayer::forward(
    const std::vector<std::vector<std::vector<double>>>& input) {

    if (input.empty()) return {};

    input_channels = input.size();
    input_height = input[0].size();
    input_width = input[0][0].size();

    if (input_height < pool_size || input_width < pool_size) {
        std::cerr << "Warning: AvgPool input " << input_height << "x" << input_width 
                  << " is smaller than pool size " << pool_size << "x" << pool_size << std::endl;
        return std::vector<std::vector<std::vector<double>>>(
            input_channels, 
            std::vector<std::vector<double>>(1, std::vector<double>(1, 0.0)));
    }

    int output_height = (input_height - pool_size) / stride + 1;
    int output_width = (input_width - pool_size) / stride + 1;

    std::vector<std::vector<std::vector<double>>> output(
        input_channels,
        std::vector<std::vector<double>>(
            output_height,
            std::vector<double>(output_width, 0.0)));

    for (int c = 0; c < input_channels; c++) {
        for (int h = 0; h < output_height; h++) {
            for (int w = 0; w < output_width; w++) {
                double sum = 0.0;
                int count = 0;

                for (int ph = 0; ph < pool_size; ph++) {
                    for (int pw = 0; pw < pool_size; pw++) {
                        int input_h = h * stride + ph;
                        int input_w = w * stride + pw;
                        
                        if (input_h < input_height && input_w < input_width) {
                            sum += input[c][input_h][input_w];
                            count++;
                        }
                    }
                }

                output[c][h][w] = (count > 0) ? sum / count : 0.0;
            }
        }
    }
    return output;
}

std::vector<std::vector<std::vector<double>>> AvgPool2DLayer::backward(
    const std::vector<std::vector<std::vector<double>>>& output_grad) {

    std::vector<std::vector<std::vector<double>>> input_grad(
        input_channels,
        std::vector<std::vector<double>>(
            input_height,
            std::vector<double>(input_width, 0.0)));

    int output_height = output_grad[0].size();
    int output_width = output_grad[0][0].size();
    double pool_area = pool_size * pool_size;

    for (int c = 0; c < input_channels; c++) {
        for (int h = 0; h < output_height; h++) {
            for (int w = 0; w < output_width; w++) {
                double grad_value = output_grad[c][h][w] / pool_area;

                for (int ph = 0; ph < pool_size; ph++) {
                    for (int pw = 0; pw < pool_size; pw++) {
                        int input_h = h * stride + ph;
                        int input_w = w * stride + pw;

                        if (input_h < input_height && input_w < input_width) {
                            input_grad[c][input_h][input_w] += grad_value;
                        }
                    }
                }
            }
        }
    }
    return input_grad;
}

int AvgPool2DLayer::get_num_parameters() const {
    return 0;
}