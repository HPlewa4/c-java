#include "model/cnn.h"
#include <cmath>
#include <random>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

ConvolutionalNeuralNetwork::ConvolutionalNeuralNetwork(double lr)
    : learning_rate(lr), batch_count(0) {

    conv1 = std::make_unique<Conv2DLayer>(3, 32, 3, 1, 1);
    pool1 = std::make_unique<MaxPool2DLayer>(2, 2);
    dropout1 = std::make_unique<DropoutLayer>(0.5);

    conv2 = std::make_unique<Conv2DLayer>(32, 64, 3, 1, 1);
    pool2 = std::make_unique<MaxPool2DLayer>(2, 2);
    dropout2 = std::make_unique<DropoutLayer>(0.5);

    conv3 = std::make_unique<Conv2DLayer>(64, 128, 3, 1, 1);
    pool3 = std::make_unique<MaxPool2DLayer>(2, 2);

    conv4 = std::make_unique<Conv2DLayer>(128, 256, 3, 1, 1);
    avgpool = std::make_unique<AvgPool2DLayer>(8, 8);

    relu = ActivationFactory::create(ActivationType::RELU);
    sigmoid = ActivationFactory::create(ActivationType::SIGMOID);

    initialize_fc_layers();

    OptimizerType opt_type = OptimizerType::ADAM;
    conv1->set_optimizer(opt_type, lr);
    conv2->set_optimizer(opt_type, lr);
    conv3->set_optimizer(opt_type, lr);
    conv4->set_optimizer(opt_type, lr);
    
    fc_weight_optimizer = OptimizerFactory::create(opt_type, lr);
    fc_bias_optimizer = OptimizerFactory::create(opt_type, lr);
}

void ConvolutionalNeuralNetwork::initialize_fc_layers() {
    std::random_device rd;
    std::mt19937 gen(rd());

    double limit = std::sqrt(6.0 / (256 + 1));
    std::uniform_real_distribution<double> dist(-limit, limit);

    fc1_weights.resize(256, std::vector<double>(1));
    fc1_weight_grads.resize(256, std::vector<double>(1, 0.0));
    
    for (int i = 0; i < 256; i++) {
        fc1_weights[i][0] = dist(gen);
    }
    
    fc1_biases.resize(1, 0.0);
    fc1_bias_grads.resize(1, 0.0);
}

std::vector<double> ConvolutionalNeuralNetwork::flatten(
    const std::vector<std::vector<std::vector<double>>>& tensor) {

    std::vector<double> result;
    result.reserve(tensor.size() * tensor[0].size() * tensor[0][0].size());
    for (const auto& channel : tensor) {
        for (const auto& row : channel) {
            for (double val : row) {
                result.push_back(val);
            }
        }
    }
    return result;
}

std::vector<std::vector<std::vector<double>>> ConvolutionalNeuralNetwork::unflatten(
    const std::vector<double>& vec, int channels, int height, int width) {

    std::vector<std::vector<std::vector<double>>> tensor(
        channels,
        std::vector<std::vector<double>>(
            height,
            std::vector<double>(width)));

    int idx = 0;
    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                tensor[c][h][w] = vec[idx++];
            }
        }
    }
    return tensor;
}

void ConvolutionalNeuralNetwork::apply_activation_3d(
    std::vector<std::vector<std::vector<double>>>& tensor,
    std::vector<std::vector<std::vector<double>>>& pre_activation,
    const ActivationFunction& activation) {

    pre_activation = tensor;
    for (auto& channel : tensor) {
        for (auto& row : channel) {
            for (double& val : row) {
                val = activation.activate(val);
            }
        }
    }
}

void ConvolutionalNeuralNetwork::apply_activation_derivative_3d(
    std::vector<std::vector<std::vector<double>>>& grad,
    const std::vector<std::vector<std::vector<double>>>& pre_activation,
    const ActivationFunction& activation) {

    for (size_t c = 0; c < grad.size(); c++) {
        for (size_t h = 0; h < grad[c].size(); h++) {
            for (size_t w = 0; w < grad[c][h].size(); w++) {
                grad[c][h][w] *= activation.derivative(pre_activation[c][h][w]);
            }
        }
    }
}

void ConvolutionalNeuralNetwork::set_training(bool is_training) {
    dropout1->set_training(is_training);
    dropout2->set_training(is_training);
}

double ConvolutionalNeuralNetwork::forward(
    const std::vector<std::vector<std::vector<double>>>& image) {

    auto out = conv1->forward(image);
    apply_activation_3d(out, conv1_pre, *relu);
    out = pool1->forward(out);
    conv1_out = out;
    pool1_out = dropout1->forward(out);
    drop1_out = pool1_out;

    out = conv2->forward(drop1_out);
    apply_activation_3d(out, conv2_pre, *relu);
    out = pool2->forward(out);
    conv2_out = out;
    pool2_out = dropout2->forward(out);
    drop2_out = pool2_out;

    out = conv3->forward(drop2_out);
    apply_activation_3d(out, conv3_pre, *relu);
    pool3_out = pool3->forward(out);

    out = conv4->forward(pool3_out);
    apply_activation_3d(out, conv4_pre, *relu);
    conv4_out = out;
    avgpool_out = avgpool->forward(out);

    flattened = flatten(avgpool_out);

    fc1_out = fc1_biases[0];
    for (size_t i = 0; i < fc1_weights.size(); i++) {
        fc1_out += flattened[i] * fc1_weights[i][0];
    }
    return sigmoid->activate(fc1_out);
}

void ConvolutionalNeuralNetwork::train_batch(
    const std::vector<std::vector<std::vector<std::vector<double>>>>& images, 
    const std::vector<int>& labels) {

    if (images.size() != labels.size()) {
        throw std::invalid_argument("Images and labels batch size mismatch");
    }

    int batch_size = images.size();

    static int debug_batch_count = 0;
    double fc_weight_before = fc1_weights[0][0];

    conv1->zero_gradients();
    conv2->zero_gradients();
    conv3->zero_gradients();
    conv4->zero_gradients();

    std::fill(fc1_bias_grads.begin(), fc1_bias_grads.end(), 0.0);
    for(auto& row : fc1_weight_grads) {
        std::fill(row.begin(), row.end(), 0.0);
    }

    for(int i = 0; i < batch_size; i++) {
        double prediction = forward(images[i]);

        if (debug_batch_count == 0 && i < batch_size) {
            std::cout << "  [DEBUG] Sample " << i << ": pred=" << prediction
                      << " label=" << labels[i] << std::endl;
        }

        double output_grad = prediction - labels[i];
                
        fc1_bias_grads[0] += output_grad;
        
        std::vector<double> flattened_grad(flattened.size());
        for(size_t j = 0; j < flattened.size(); j++) {
            fc1_weight_grads[j][0] += output_grad * flattened[j];
            flattened_grad[j] = output_grad * fc1_weights[j][0];
        }
        
        auto avgpool_grad = unflatten(flattened_grad, 256, 1, 1);
        
        auto conv4_post_grad = avgpool->backward(avgpool_grad);
        
        if (debug_batch_count == 0 && i == 0) {
            double sum_before = 0;
            for (const auto& c : conv4_post_grad) for (const auto& r : c) for (double v : r) sum_before += std::abs(v);
            std::cout << "  [DEBUG] Conv4 grad sum before ReLU deriv: " << sum_before << std::endl;
        }
        apply_activation_derivative_3d(conv4_post_grad, conv4_pre, *relu);
        if (debug_batch_count == 0 && i == 0) {
            double sum_after = 0;
            for (const auto& c : conv4_post_grad) for (const auto& r : c) for (double v : r) sum_after += std::abs(v);
            std::cout << "  [DEBUG] Conv4 grad sum after ReLU deriv: " << sum_after << std::endl;
        }
        auto pool3_post_grad = conv4->backward(conv4_post_grad);

        auto conv3_post_grad = pool3->backward(pool3_post_grad);
        apply_activation_derivative_3d(conv3_post_grad, conv3_pre, *relu);
        auto drop2_post_grad = conv3->backward(conv3_post_grad);

        auto pool2_post_grad = dropout2->backward(drop2_post_grad);
        auto conv2_post_grad = pool2->backward(pool2_post_grad);
        apply_activation_derivative_3d(conv2_post_grad, conv2_pre, *relu);
        auto drop1_post_grad = conv2->backward(conv2_post_grad);

        auto pool1_post_grad = dropout1->backward(drop1_post_grad);
        auto conv1_post_grad = pool1->backward(pool1_post_grad);
        apply_activation_derivative_3d(conv1_post_grad, conv1_pre, *relu);
        if (debug_batch_count == 0 && i == 0) {
            double sum_final = 0;
            for (const auto& c : conv1_post_grad) for (const auto& r : c) for (double v : r) sum_final += std::abs(v);
            std::cout << "  [DEBUG] Conv1 grad sum final: " << sum_final << std::endl;
        }
        conv1->backward(conv1_post_grad);
    }
    
    conv1->apply_gradients();
    conv2->apply_gradients();
    conv3->apply_gradients();
    conv4->apply_gradients();

    double scale = 1.0 / batch_size;
    fc1_bias_grads[0] *= scale;
    for(auto& row : fc1_weight_grads) {
        for(auto& val : row) val *= scale;
    }

    if (debug_batch_count == 0) {
        std::cout << "  [DEBUG] FC bias grad: " << fc1_bias_grads[0] << std::endl;
        std::cout << "  [DEBUG] FC weight[0] grad: " << fc1_weight_grads[0][0] << std::endl;
        std::cout << "  [DEBUG] FC weight[0] before: " << fc_weight_before << std::endl;
    }

    fc_weight_optimizer->update_2d(fc1_weights, fc1_weight_grads);
    fc_bias_optimizer->update_1d(fc1_biases, fc1_bias_grads);

    if (debug_batch_count == 0) {
        double fc_weight_after = fc1_weights[0][0];
        std::cout << "  [DEBUG] FC weight[0] after: " << fc_weight_after
                  << " (change: " << (fc_weight_after - fc_weight_before) << ")" << std::endl;
    }

    debug_batch_count++;
}

void ConvolutionalNeuralNetwork::train(
    const std::vector<std::vector<std::vector<double>>>& image, int label) {
    std::vector<std::vector<std::vector<std::vector<double>>>> images = {image};
    std::vector<int> labels = {label};
    train_batch(images, labels);
}

int ConvolutionalNeuralNetwork::predict(
    const std::vector<std::vector<std::vector<double>>>& image) {
    set_training(false);
    double prob = forward(image);
    return (prob >= 0.5) ? 1 : 0;
}

void ConvolutionalNeuralNetwork::save(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    int config_id = 20240110;
    file.write(reinterpret_cast<const char*>(&config_id), sizeof(int));

    file.write(reinterpret_cast<const char*>(&learning_rate), sizeof(double));

    conv1->save(file);
    conv2->save(file);
    conv3->save(file);
    conv4->save(file);

    for (int i = 0; i < 256; i++) {
        file.write(reinterpret_cast<const char*>(&fc1_weights[i][0]), sizeof(double));
    }
    file.write(reinterpret_cast<const char*>(&fc1_biases[0]), sizeof(double));

    file.close();
    std::cout << "Model saved to " << filename << std::endl;
}

void ConvolutionalNeuralNetwork::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }

    int config_id;
    file.read(reinterpret_cast<char*>(&config_id), sizeof(int));
    if (config_id != 20240110) {
        throw std::runtime_error("Incompatible model version. Expected 20240110, got " + std::to_string(config_id));
    }

    file.read(reinterpret_cast<char*>(&learning_rate), sizeof(double));

    conv1->load(file);
    conv2->load(file);
    conv3->load(file);
    conv4->load(file);

    fc1_weights.resize(256, std::vector<double>(1));
    fc1_biases.resize(1);
    for (int i = 0; i < 256; i++) {
        file.read(reinterpret_cast<char*>(&fc1_weights[i][0]), sizeof(double));
    }
    file.read(reinterpret_cast<char*>(&fc1_biases[0]), sizeof(double));

    file.close();
    std::cout << "Model loaded from " << filename << std::endl;
}

int ConvolutionalNeuralNetwork::get_num_parameters() const {
    int conv_params = conv1->get_num_parameters() + 
                      conv2->get_num_parameters() +
                      conv3->get_num_parameters() +
                      conv4->get_num_parameters();
    int fc_params = 256 * 1 + 1;
    return conv_params + fc_params;
}