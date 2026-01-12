#include "../../include/model/dropout_layer.h"
#include <ctime>

DropoutLayer::DropoutLayer(double rate)
    : dropout_rate(rate), is_training(true), channels(0), height(0), width(0) {
    rng.seed(static_cast<unsigned int>(std::time(nullptr)));
    distribution = std::bernoulli_distribution(1.0 - dropout_rate);
}

std::vector<std::vector<std::vector<double>>> DropoutLayer::forward(
    const std::vector<std::vector<std::vector<double>>>& input,
    bool training) {

    is_training = training;
    channels = input.size();
    height = input[0].size();
    width = input[0][0].size();

    std::vector<std::vector<std::vector<double>>> output(
        channels,
        std::vector<std::vector<double>>(
            height,
            std::vector<double>(width, 0.0)));

    if (!is_training) {
        return input;
    }

    mask.resize(channels,
        std::vector<std::vector<bool>>(
            height,
            std::vector<bool>(width, false)));

    double scale = 1.0 / (1.0 - dropout_rate);

    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                mask[c][h][w] = distribution(rng);

                if (mask[c][h][w]) {
                    output[c][h][w] = input[c][h][w] * scale;
                } else {
                    output[c][h][w] = 0.0;
                }
            }
        }
    }
    return output;
}

std::vector<std::vector<std::vector<double>>> DropoutLayer::backward(
    const std::vector<std::vector<std::vector<double>>>& output_grad) {

    std::vector<std::vector<std::vector<double>>> input_grad(
        channels,
        std::vector<std::vector<double>>(
            height,
            std::vector<double>(width, 0.0)));

    if (!is_training) {
        return output_grad;
    }

    double scale = 1.0 / (1.0 - dropout_rate);
    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                if (mask[c][h][w]) {
                    input_grad[c][h][w] = output_grad[c][h][w] * scale;
                } else {
                    input_grad[c][h][w] = 0.0;
                }
            }
        }
    }
    return input_grad;
}

void DropoutLayer::set_training(bool training) {
    is_training = training;
}

int DropoutLayer::get_num_parameters() const {
    return 0;
}