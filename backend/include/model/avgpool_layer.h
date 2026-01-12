#ifndef AVGPOOL_LAYER_H
#define AVGPOOL_LAYER_H

#include <vector>

class AvgPool2DLayer {
private:
    int pool_size;
    int stride;
    int input_channels, input_height, input_width;

public:
    explicit AvgPool2DLayer(int pool_size, int stride = -1);
    std::vector<std::vector<std::vector<double>>> forward(
        const std::vector<std::vector<std::vector<double>>>& input);

    std::vector<std::vector<std::vector<double>>> backward(
        const std::vector<std::vector<std::vector<double>>>& output_grad);

    int get_num_parameters() const;
};

#endif