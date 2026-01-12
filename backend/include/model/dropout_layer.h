#ifndef DROPOUT_LAYER_H
#define DROPOUT_LAYER_H

#include <vector>
#include <random>

class DropoutLayer {
private:
    double dropout_rate;
    bool is_training;

    std::vector<std::vector<std::vector<bool>>> mask;
    std::mt19937 rng;
    std::bernoulli_distribution distribution;

    int channels, height, width;

public:
    explicit DropoutLayer(double rate = 0.5);
    std::vector<std::vector<std::vector<double>>> forward(
        const std::vector<std::vector<std::vector<double>>>& input,
        bool training = true);
    std::vector<std::vector<std::vector<double>>> backward(
        const std::vector<std::vector<std::vector<double>>>& output_grad);

    void set_training(bool training);
    int get_num_parameters() const;
};

#endif