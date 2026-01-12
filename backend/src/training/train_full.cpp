#include "model/cnn.h"
#include "utils/augmentation.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

struct CelebASample {
    std::string path;
    int label;
};

std::vector<CelebASample> load_sample_list(const std::string& split_file, int max_samples = -1) {
    std::vector<CelebASample> samples;
    std::ifstream file(split_file);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open split file: " + split_file);
    }

    std::string line;
    int loaded = 0;

    while (std::getline(file, line)) {
        if (max_samples > 0 && loaded >= max_samples) {
            break;
        }

        std::istringstream iss(line);
        std::string img_path;
        int label;

        if (!(iss >> img_path >> label)) {
            continue;
        }

        samples.push_back({img_path, label});
        loaded++;
    }

    return samples;
}

std::vector<std::vector<std::vector<double>>> load_image_train(
    const std::string& full_path, ImageAugmentation& aug) {

    cv::Mat img = cv::imread(full_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + full_path);
    }

    return aug.training_transform(img, 64);
}

std::vector<std::vector<std::vector<double>>> load_image_val(
    const std::string& full_path, ImageAugmentation& aug) {

    cv::Mat img = cv::imread(full_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + full_path);
    }

    return aug.validation_transform(img, 64);
}

double evaluate(ConvolutionalNeuralNetwork& model,
                const std::vector<CelebASample>& samples,
                const std::string& base_path,
                ImageAugmentation& aug) {
    model.set_training(false);
    int correct = 0;
    int total = 0;

    for (const auto& sample : samples) {
        std::string full_path = base_path + "/" + sample.path;

        try {
            auto image = load_image_val(full_path, aug);
            int pred = model.predict(image);

            if (pred == sample.label) {
                correct++;
            }
            total++;
        } catch (const std::exception& e) {
            continue;
        }
    }

    return (total > 0) ? (100.0 * correct / total) : 0.0;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  CPU CNN Training (Pure C++)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::string base_path = "../src/dataset";
    std::string train_split = base_path + "/data/celeba_splits/train.txt";
    std::string val_split = base_path + "/data/celeba_splits/val.txt";
    std::string model_file = "../models/cnn_cpu.bin";

    int epochs = 30;
    double learning_rate = 0.001;
    int batch_size = 32;

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Epochs: " << epochs << std::endl;
    std::cout << "  Learning rate: " << learning_rate << std::endl;
    std::cout << "  Batch size: " << batch_size << std::endl;
    std::cout << "  Optimizer: Adam" << std::endl;
    std::cout << "  Device: CPU" << std::endl;
    std::cout << std::endl;

    std::cout << "Loading datasets..." << std::endl;
    auto train_samples = load_sample_list(train_split);
    auto val_samples = load_sample_list(val_split);
    std::cout << "  Training samples: " << train_samples.size() << std::endl;
    std::cout << "  Validation samples: " << val_samples.size() << std::endl;
    std::cout << std::endl;

    ConvolutionalNeuralNetwork model(learning_rate);
    int num_params = model.get_num_parameters();
    std::cout << "Model: 4-Layer CNN with " << num_params << " parameters" << std::endl;
    std::cout << std::endl;

    ImageAugmentation aug;

    std::cout << "Starting training..." << std::endl;
    std::cout << "========================================" << std::endl;
    auto training_start = std::chrono::high_resolution_clock::now();

    for (int epoch = 0; epoch < epochs; epoch++) {
        model.set_training(true);
        auto epoch_start = std::chrono::high_resolution_clock::now();

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(train_samples.begin(), train_samples.end(), g);

        int processed_samples = 0;

        for (size_t i = 0; i < train_samples.size(); i += batch_size) {
            size_t end = std::min(i + batch_size, train_samples.size());
            size_t current_batch_size = end - i;

            std::vector<std::vector<std::vector<std::vector<double>>>> batch_images;
            std::vector<int> batch_labels;

            for (size_t j = 0; j < current_batch_size; j++) {
                const auto& sample = train_samples[i + j];
                std::string full_path = base_path + "/" + sample.path;

                try {
                    auto img = load_image_train(full_path, aug);
                    batch_images.push_back(img);
                    batch_labels.push_back(sample.label);
                } catch (const std::exception& e) {
                    continue;
                }
            }

            if (batch_images.empty()) continue;

            try {
                model.train_batch(batch_images, batch_labels);
                processed_samples += batch_images.size();
            } catch (const std::exception& e) {
                std::cerr << "\nError training batch: " << e.what() << std::endl;
                continue;
            }

            if ((i / batch_size + 1) % 10 == 0) {
                auto current_time = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    current_time - epoch_start).count();
                double samples_per_sec = (elapsed > 0) ?
                    static_cast<double>(processed_samples) / elapsed : 0.0;

                std::cout << "\r  Epoch " << (epoch + 1) << "/" << epochs
                          << " - " << processed_samples << "/" << train_samples.size()
                          << " samples (" << std::fixed << std::setprecision(1)
                          << samples_per_sec << " samples/sec)";
                std::cout.flush();
            }
        }

        auto epoch_end = std::chrono::high_resolution_clock::now();
        auto epoch_duration = std::chrono::duration_cast<std::chrono::seconds>(
            epoch_end - epoch_start).count();

        std::cout << "\r  Epoch " << (epoch + 1) << "/" << epochs
                  << " completed in " << epoch_duration << "s" << std::endl;

        double val_acc = evaluate(model, val_samples, base_path, aug);
        std::cout << "    Validation Accuracy: " << std::fixed << std::setprecision(2)
                  << val_acc << "%" << std::endl;

        if ((epoch + 1) % 5 == 0) {
            try {
                model.save(model_file);
                std::cout << "    Checkpoint saved" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "    Error saving checkpoint: " << e.what() << std::endl;
            }
        }
    }

    auto training_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::minutes>(
        training_end - training_start).count();

    std::cout << "========================================" << std::endl;
    std::cout << "Training completed in " << total_duration << " minutes" << std::endl;

    try {
        model.save(model_file);
        std::cout << "Model saved to " << model_file << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error saving final model: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}