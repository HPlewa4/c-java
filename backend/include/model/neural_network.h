#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <memory>
#include <thread>
#include <mutex>
#include "activation_function.h"

class NeuralNetwork {
private:
    std::vector<int> layers;
    std::vector<std::vector<std::vector<double>>> weights;
    std::vector<std::vector<double>> biases;
    double learning_rate;

    // Polymorphism - using activation function via base class pointer
    std::unique_ptr<ActivationFunction> activation;

    // Mutex for thread safety in parallel training
    std::mutex training_mutex;

    double sigmoid(double x);
    double sigmoid_derivative(double x);
    std::vector<double> softmax(const std::vector<double>& x);

public:
    NeuralNetwork(const std::vector<int>& layer_sizes, double lr = 0.01,
                  ActivationType act_type = ActivationType::SIGMOID);

    // Destructor - cleanup resources
    ~NeuralNetwork();

    std::vector<double> forward(const std::vector<double>& input);
    void train(const std::vector<double>& input, const std::vector<double>& target);
    void train_batch(const std::vector<std::vector<double>>& inputs,
                     const std::vector<std::vector<double>>& targets,
                     int epochs);

    // Parallel training using std::thread
    void train_batch_parallel(const std::vector<std::vector<double>>& inputs,
                             const std::vector<std::vector<double>>& targets,
                             int epochs, int num_threads = 4);

    void save(const std::string& filename);
    void load(const std::string& filename);

    int predict_class(const std::vector<double>& input);

    // Get activation type
    ActivationType getActivationType() const;
};

#endif
