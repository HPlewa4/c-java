#ifndef CONV_LAYER_H
#define CONV_LAYER_H

#include <vector>
#include <random>
#include <memory>
#include "../optimizer/optimizer.h"

class Conv2DLayer {
private:
    int in_channels;
    int out_channels;
    int kernel_size;
    int stride;
    int padding;

    std::vector<std::vector<std::vector<std::vector<double>>>> weights;
    std::vector<double> biases;
    std::vector<std::vector<std::vector<std::vector<double>>>> weight_grad_accum;
    std::vector<double> bias_grad_accum;
    int batch_accumulation_count;

    std::unique_ptr<Optimizer> weight_optimizer;
    std::unique_ptr<Optimizer> bias_optimizer;
    std::vector<std::vector<std::vector<double>>> last_input;
    std::mt19937 rng;

    std::vector<double> im2col_data;
    int last_output_height, last_output_width;

    std::vector<std::vector<std::vector<double>>> apply_padding(
        const std::vector<std::vector<std::vector<double>>>& input) const;
    std::vector<double> im2col(
        const std::vector<std::vector<std::vector<double>>>& input,
        int output_height, int output_width) const;
    std::vector<std::vector<std::vector<double>>> col2im(
        const std::vector<double>& col,
        int input_channels, int input_height, int input_width,
        int output_height, int output_width) const;

public:
    Conv2DLayer(int in_ch, int out_ch, int k_size, int stride = 1, int pad = 1);
    std::vector<std::vector<std::vector<double>>> forward(
        const std::vector<std::vector<std::vector<double>>>& input);

    std::vector<std::vector<std::vector<double>>> backward(
        const std::vector<std::vector<std::vector<double>>>& output_grad);

    void zero_gradients();
    void apply_gradients();
    void set_optimizer(OptimizerType opt_type, double learning_rate);
    int get_num_parameters() const {
        return out_channels * in_channels * kernel_size * kernel_size + out_channels;
    }
    void save(std::ofstream& file) const;
    void load(std::ifstream& file);
};

#endif