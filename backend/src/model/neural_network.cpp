#include "neural_network.h"
#include <fstream>
#include <iostream>
#include <algorithm>

NeuralNetwork::NeuralNetwork(const std::vector<int>& layer_sizes, double lr,
                             ActivationType act_type)
    : layers(layer_sizes), learning_rate(lr) {

    // Polymorphism - create activation function using factory
    activation = ActivationFactory::create(act_type);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, 0.1);

    for (size_t i = 0; i < layers.size() - 1; i++) {
        std::vector<std::vector<double>> layer_weights;
        std::vector<double> layer_biases;

        for (int j = 0; j < layers[i + 1]; j++) {
            std::vector<double> neuron_weights;
            for (int k = 0; k < layers[i]; k++) {
                neuron_weights.push_back(d(gen));
            }
            layer_weights.push_back(neuron_weights);
            layer_biases.push_back(d(gen));
        }

        weights.push_back(layer_weights);
        biases.push_back(layer_biases);
    }
}

// Destructor - cleanup
NeuralNetwork::~NeuralNetwork() {
    // Smart pointers (unique_ptr) automatically clean up
    // This destructor is here to demonstrate proper resource management
    weights.clear();
    biases.clear();
}

double NeuralNetwork::sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double NeuralNetwork::sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

std::vector<double> NeuralNetwork::softmax(const std::vector<double>& x) {
    std::vector<double> result(x.size());
    double max_val = *std::max_element(x.begin(), x.end());
    double sum = 0.0;
    
    for (size_t i = 0; i < x.size(); i++) {
        result[i] = exp(x[i] - max_val);
        sum += result[i];
    }
    
    for (size_t i = 0; i < result.size(); i++) {
        result[i] /= sum;
    }
    
    return result;
}

std::vector<double> NeuralNetwork::forward(const std::vector<double>& input) {
    std::vector<double> activation = input;
    
    for (size_t layer = 0; layer < weights.size(); layer++) {
        std::vector<double> new_activation;
        
        for (size_t neuron = 0; neuron < weights[layer].size(); neuron++) {
            double sum = biases[layer][neuron];
            for (size_t i = 0; i < activation.size(); i++) {
                sum += activation[i] * weights[layer][neuron][i];
            }
            
            if (layer == weights.size() - 1) {
                new_activation.push_back(sum);
            } else {
                new_activation.push_back(sigmoid(sum));
            }
        }
        
        activation = new_activation;
    }
    
    return softmax(activation);
}

void NeuralNetwork::train(const std::vector<double>& input, const std::vector<double>& target) {
    std::vector<std::vector<double>> activations;
    std::vector<std::vector<double>> z_values;
    
    activations.push_back(input);
    
    std::vector<double> activation = input;
    for (size_t layer = 0; layer < weights.size(); layer++) {
        std::vector<double> z;
        std::vector<double> new_activation;
        
        for (size_t neuron = 0; neuron < weights[layer].size(); neuron++) {
            double sum = biases[layer][neuron];
            for (size_t i = 0; i < activation.size(); i++) {
                sum += activation[i] * weights[layer][neuron][i];
            }
            z.push_back(sum);
            
            if (layer == weights.size() - 1) {
                new_activation.push_back(sum);
            } else {
                new_activation.push_back(sigmoid(sum));
            }
        }
        
        z_values.push_back(z);
        if (layer == weights.size() - 1) {
            new_activation = softmax(new_activation);
        }
        activations.push_back(new_activation);
        activation = new_activation;
    }
    
    std::vector<std::vector<double>> deltas(weights.size());
    
    std::vector<double> output_error(target.size());
    for (size_t i = 0; i < target.size(); i++) {
        output_error[i] = activations.back()[i] - target[i];
    }
    deltas[deltas.size() - 1] = output_error;
    
    for (int layer = weights.size() - 2; layer >= 0; layer--) {
        std::vector<double> error(weights[layer].size(), 0.0);
        
        for (size_t i = 0; i < weights[layer].size(); i++) {
            for (size_t j = 0; j < weights[layer + 1].size(); j++) {
                error[i] += deltas[layer + 1][j] * weights[layer + 1][j][i];
            }
            error[i] *= sigmoid_derivative(activations[layer + 1][i]);
        }
        deltas[layer] = error;
    }
    
    for (size_t layer = 0; layer < weights.size(); layer++) {
        for (size_t neuron = 0; neuron < weights[layer].size(); neuron++) {
            for (size_t w = 0; w < weights[layer][neuron].size(); w++) {
                weights[layer][neuron][w] -= learning_rate * deltas[layer][neuron] * activations[layer][w];
            }
            biases[layer][neuron] -= learning_rate * deltas[layer][neuron];
        }
    }
}

void NeuralNetwork::train_batch(const std::vector<std::vector<double>>& inputs, 
                                 const std::vector<std::vector<double>>& targets, 
                                 int epochs) {
    for (int epoch = 0; epoch < epochs; epoch++) {
        double total_loss = 0.0;
        
        for (size_t i = 0; i < inputs.size(); i++) {
            train(inputs[i], targets[i]);
            
            auto output = forward(inputs[i]);
            for (size_t j = 0; j < output.size(); j++) {
                total_loss += -targets[i][j] * log(output[j] + 1e-10);
            }
        }
        
        if ((epoch + 1) % 10 == 0) {
            std::cout << "Epoch " << epoch + 1 << "/" << epochs 
                      << " - Loss: " << total_loss / inputs.size() << std::endl;
        }
    }
}

void NeuralNetwork::save(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    
    size_t num_layers = layers.size();
    file.write((char*)&num_layers, sizeof(size_t));
    file.write((char*)layers.data(), layers.size() * sizeof(int));
    file.write((char*)&learning_rate, sizeof(double));
    
    for (size_t i = 0; i < weights.size(); i++) {
        for (size_t j = 0; j < weights[i].size(); j++) {
            file.write((char*)weights[i][j].data(), weights[i][j].size() * sizeof(double));
        }
        file.write((char*)biases[i].data(), biases[i].size() * sizeof(double));
    }
    
    file.close();
    std::cout << "Model saved to " << filename << std::endl;
}

void NeuralNetwork::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open model file: " << filename << std::endl;
        return;
    }
    
    size_t num_layers;
    file.read((char*)&num_layers, sizeof(size_t));
    layers.resize(num_layers);
    file.read((char*)layers.data(), layers.size() * sizeof(int));
    file.read((char*)&learning_rate, sizeof(double));
    
    weights.clear();
    biases.clear();
    
    for (size_t i = 0; i < layers.size() - 1; i++) {
        std::vector<std::vector<double>> layer_weights;
        std::vector<double> layer_biases(layers[i + 1]);
        
        for (int j = 0; j < layers[i + 1]; j++) {
            std::vector<double> neuron_weights(layers[i]);
            file.read((char*)neuron_weights.data(), neuron_weights.size() * sizeof(double));
            layer_weights.push_back(neuron_weights);
        }
        file.read((char*)layer_biases.data(), layer_biases.size() * sizeof(double));
        
        weights.push_back(layer_weights);
        biases.push_back(layer_biases);
    }
    
    file.close();
    std::cout << "Model loaded from " << filename << std::endl;
}

int NeuralNetwork::predict_class(const std::vector<double>& input) {
    auto output = forward(input);
    return std::max_element(output.begin(), output.end()) - output.begin();
}

ActivationType NeuralNetwork::getActivationType() const {
    return activation->getType();
}

// Parallel training implementation using std::thread
void NeuralNetwork::train_batch_parallel(const std::vector<std::vector<double>>& inputs,
                                        const std::vector<std::vector<double>>& targets,
                                        int epochs, int num_threads) {
    for (int epoch = 0; epoch < epochs; epoch++) {
        double total_loss = 0.0;
        std::vector<std::thread> threads;
        std::vector<double> thread_losses(num_threads, 0.0);

        // Divide work among threads
        size_t batch_per_thread = inputs.size() / num_threads;

        for (int t = 0; t < num_threads; t++) {
            size_t start_idx = t * batch_per_thread;
            size_t end_idx = (t == num_threads - 1) ? inputs.size() : (t + 1) * batch_per_thread;

            threads.emplace_back([this, &inputs, &targets, &thread_losses, t, start_idx, end_idx]() {
                double local_loss = 0.0;

                for (size_t i = start_idx; i < end_idx; i++) {
                    // Lock for thread-safe training
                    {
                        std::lock_guard<std::mutex> lock(training_mutex);
                        train(inputs[i], targets[i]);
                    }

                    // Calculate loss without lock
                    auto output = forward(inputs[i]);
                    for (size_t j = 0; j < output.size(); j++) {
                        local_loss += -targets[i][j] * log(output[j] + 1e-10);
                    }
                }

                thread_losses[t] = local_loss;
            });
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }

        // Aggregate losses
        for (double loss : thread_losses) {
            total_loss += loss;
        }

        if ((epoch + 1) % 10 == 0) {
            std::cout << "Epoch " << epoch + 1 << "/" << epochs
                      << " - Loss: " << total_loss / inputs.size() << std::endl;
        }
    }
}
