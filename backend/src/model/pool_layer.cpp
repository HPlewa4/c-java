#include "model/pool_layer.h"
#include <stdexcept>
#include <limits>

MaxPool2DLayer::MaxPool2DLayer(int pool_size, int stride)
    : pool_size(pool_size), stride(stride) {}

std::vector<std::vector<std::vector<double>>> MaxPool2DLayer::forward(
    const std::vector<std::vector<std::vector<double>>>& input) {

    if (input.empty() || input[0].empty() || input[0][0].empty()) {
        throw std::invalid_argument("MaxPool2DLayer: empty input");
    }

    int channels = input.size();
    int input_height = input[0].size();
    int input_width = input[0][0].size();

    int out_height = (input_height - pool_size) / stride + 1;
    int out_width = (input_width - pool_size) / stride + 1;

    std::vector<std::vector<std::vector<double>>> output(
        channels,
        std::vector<std::vector<double>>(
            out_height,
            std::vector<double>(out_width)));

    max_indices.resize(channels);
    for (int c = 0; c < channels; c++) {
        max_indices[c].resize(out_height);
        for (int h = 0; h < out_height; h++) {
            max_indices[c][h].resize(out_width);
        }
    }

    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < out_height; h++) {
            for (int w = 0; w < out_width; w++) {
                double max_val = -std::numeric_limits<double>::infinity();
                int max_h = 0, max_w = 0;

                for (int ph = 0; ph < pool_size; ph++) {
                    for (int pw = 0; pw < pool_size; pw++) {
                        int input_h = h * stride + ph;
                        int input_w = w * stride + pw;

                        double val = input[c][input_h][input_w];
                        if (val > max_val) {
                            max_val = val;
                            max_h = input_h;
                            max_w = input_w;
                        }
                    }
                }

                output[c][h][w] = max_val;
                max_indices[c][h][w] = {max_h, max_w};
            }
        }
    }

    return output;
}

std::vector<std::vector<std::vector<double>>> MaxPool2DLayer::backward(
    const std::vector<std::vector<std::vector<double>>>& output_grad) {

    if (max_indices.empty()) {
        throw std::runtime_error("MaxPool2DLayer: backward called before forward");
    }

    int channels = output_grad.size();
    int out_height = output_grad[0].size();
    int out_width = output_grad[0][0].size();

    int input_height = (out_height - 1) * stride + pool_size;
    int input_width = (out_width - 1) * stride + pool_size;

    std::vector<std::vector<std::vector<double>>> input_grad(
        channels,
        std::vector<std::vector<double>>(
            input_height,
            std::vector<double>(input_width, 0.0)));

    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < out_height; h++) {
            for (int w = 0; w < out_width; w++) {
                int max_h = max_indices[c][h][w].first;
                int max_w = max_indices[c][h][w].second;

                input_grad[c][max_h][max_w] += output_grad[c][h][w];
            }
        }
    }

    return input_grad;
}