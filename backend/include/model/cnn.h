#ifndef CNN_H
#define CNN_H

#include "model/conv_layer.h"
#include "model/pool_layer.h"
#include "model/avgpool_layer.h"
#include "model/dropout_layer.h"
#include "model/activation/activation_function.h"
#include "optimizer/optimizer.h"
#include <vector>
#include <memory>
#include <string>

class ConvolutionalNeuralNetwork {
private:
    std::unique_ptr<Conv2DLayer> conv1;
    std::unique_ptr<MaxPool2DLayer> pool1;
    std::unique_ptr<DropoutLayer> dropout1;

    std::unique_ptr<Conv2DLayer> conv2;
    std::unique_ptr<MaxPool2DLayer> pool2;
    std::unique_ptr<DropoutLayer> dropout2;

    std::unique_ptr<Conv2DLayer> conv3;
    std::unique_ptr<MaxPool2DLayer> pool3;

    std::unique_ptr<Conv2DLayer> conv4;
    std::unique_ptr<AvgPool2DLayer> avgpool;

    std::vector<std::vector<double>> fc1_weights;
    std::vector<double> fc1_biases;

    std::vector<std::vector<double>> fc1_weight_grads;
    std::vector<double> fc1_bias_grads;
    int batch_count;

    std::unique_ptr<Optimizer> fc_weight_optimizer;
    std::unique_ptr<Optimizer> fc_bias_optimizer;

    double learning_rate;

    std::unique_ptr<ActivationFunction> relu;
    std::unique_ptr<ActivationFunction> sigmoid;

    std::vector<std::vector<std::vector<double>>> conv1_out;
    std::vector<std::vector<std::vector<double>>> pool1_out;
    std::vector<std::vector<std::vector<double>>> drop1_out;
    
    std::vector<std::vector<std::vector<double>>> conv2_out;
    std::vector<std::vector<std::vector<double>>> pool2_out;
    std::vector<std::vector<std::vector<double>>> drop2_out;
    
    std::vector<std::vector<std::vector<double>>> conv3_out;
    std::vector<std::vector<std::vector<double>>> pool3_out;
    
    std::vector<std::vector<std::vector<double>>> conv4_out;
    std::vector<std::vector<std::vector<double>>> avgpool_out;
    
    std::vector<double> flattened;
    double fc1_out;

    std::vector<std::vector<std::vector<double>>> conv1_pre;
    std::vector<std::vector<std::vector<double>>> conv2_pre;
    std::vector<std::vector<std::vector<double>>> conv3_pre;
    std::vector<std::vector<std::vector<double>>> conv4_pre;

    std::vector<double> flatten(
        const std::vector<std::vector<std::vector<double>>>& tensor);

    std::vector<std::vector<std::vector<double>>> unflatten(
        const std::vector<double>& vec, int channels, int height, int width);

    void apply_activation_3d(
        std::vector<std::vector<std::vector<double>>>& tensor,
        std::vector<std::vector<std::vector<double>>>& pre_activation,
        const ActivationFunction& activation);

    void apply_activation_derivative_3d(
        std::vector<std::vector<std::vector<double>>>& grad,
        const std::vector<std::vector<std::vector<double>>>& pre_activation,
        const ActivationFunction& activation);

    void initialize_fc_layers();

public:
    ConvolutionalNeuralNetwork(double lr = 0.001);

    double forward(const std::vector<std::vector<std::vector<double>>>& image);
    void train_batch(const std::vector<std::vector<std::vector<std::vector<double>>>>& images, 
                     const std::vector<int>& labels);
    void train(const std::vector<std::vector<std::vector<double>>>& image, int label);
    void set_training(bool is_training);
    int predict(const std::vector<std::vector<std::vector<double>>>& image);
    void save(const std::string& filename);
    void load(const std::string& filename);
    int get_num_parameters() const;
};

#endif