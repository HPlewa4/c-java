#ifndef POOL_LAYER_H
#define POOL_LAYER_H

#include <vector>
#include <utility>

class MaxPool2DLayer {
private:
    int pool_size;
    int stride;
    std::vector<std::vector<std::vector<std::pair<int, int>>>> max_indices;

public:
    MaxPool2DLayer(int pool_size = 2, int stride = 2);
    std::vector<std::vector<std::vector<double>>> forward(
        const std::vector<std::vector<std::vector<double>>>& input);
    std::vector<std::vector<std::vector<double>>> backward(
        const std::vector<std::vector<std::vector<double>>>& output_grad);

    int get_num_parameters() const {
        return 0;
    }
};

#endif