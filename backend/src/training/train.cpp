#include "neural_network.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

struct Dataset {
    std::vector<std::vector<double>> images;
    std::vector<int> labels;
    std::vector<std::string> class_names;
};

std::vector<double> image_to_vector(const cv::Mat& img, int target_size = 32) {
    cv::Mat resized, gray;
    cv::resize(img, resized, cv::Size(target_size, target_size));
    cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
    
    std::vector<double> vec;
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            vec.push_back(gray.at<uchar>(i, j) / 255.0);
        }
    }
    return vec;
}

Dataset load_dataset(const std::string& data_dir, int img_size = 32) {
    Dataset dataset;
    
    std::vector<std::string> class_dirs;
    for (const auto& entry : fs::directory_iterator(data_dir)) {
        if (entry.is_directory()) {
            class_dirs.push_back(entry.path().filename().string());
        }
    }
    std::sort(class_dirs.begin(), class_dirs.end());
    dataset.class_names = class_dirs;
    
    std::cout << "Found " << class_dirs.size() << " classes:" << std::endl;
    for (const auto& name : class_dirs) {
        std::cout << "  - " << name << std::endl;
    }
    
    for (size_t class_idx = 0; class_idx < class_dirs.size(); class_idx++) {
        std::string class_path = data_dir + "/" + class_dirs[class_idx];
        int count = 0;
        
        for (const auto& entry : fs::directory_iterator(class_path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
                    cv::Mat img = cv::imread(entry.path().string());
                    if (!img.empty()) {
                        dataset.images.push_back(image_to_vector(img, img_size));
                        dataset.labels.push_back(class_idx);
                        count++;
                    }
                }
            }
        }
        std::cout << "Loaded " << count << " images from class: " << class_dirs[class_idx] << std::endl;
    }
    
    return dataset;
}

std::vector<double> one_hot_encode(int label, int num_classes) {
    std::vector<double> encoded(num_classes, 0.0);
    encoded[label] = 1.0;
    return encoded;
}

int main(int argc, char* argv[]) {
    std::string data_dir = "../data/train";
    std::string model_file = "../models/trained_model.bin";
    int img_size = 32;
    int epochs = 100;
    
    if (argc > 1) data_dir = argv[1];
    if (argc > 2) model_file = argv[2];
    if (argc > 3) img_size = std::atoi(argv[3]);
    if (argc > 4) epochs = std::atoi(argv[4]);
    
    std::cout << "=== Neural Network Trainer ===" << std::endl;
    std::cout << "Data directory: " << data_dir << std::endl;
    std::cout << "Model output: " << model_file << std::endl;
    std::cout << "Image size: " << img_size << "x" << img_size << std::endl;
    std::cout << "Epochs: " << epochs << std::endl;
    std::cout << std::endl;
    
    std::cout << "Loading dataset..." << std::endl;
    Dataset dataset = load_dataset(data_dir, img_size);
    
    if (dataset.images.empty()) {
        std::cerr << "No images loaded! Check your data directory." << std::endl;
        return 1;
    }
    
    std::cout << "Total images: " << dataset.images.size() << std::endl;
    std::cout << "Number of classes: " << dataset.class_names.size() << std::endl;
    std::cout << std::endl;
    
    std::vector<std::vector<double>> targets;
    for (int label : dataset.labels) {
        targets.push_back(one_hot_encode(label, dataset.class_names.size()));
    }
    
    int input_size = img_size * img_size;
    int hidden_size = 128;
    int output_size = dataset.class_names.size();
    
    std::cout << "Creating neural network..." << std::endl;
    std::cout << "Architecture: " << input_size << " -> " << hidden_size << " -> " << output_size << std::endl;
    std::cout << std::endl;
    
    NeuralNetwork nn({input_size, hidden_size, output_size}, 0.01);
    
    std::cout << "Training..." << std::endl;
    nn.train_batch(dataset.images, targets, epochs);
    
    std::cout << "\nEvaluating on training set..." << std::endl;
    int correct = 0;
    for (size_t i = 0; i < dataset.images.size(); i++) {
        int predicted = nn.predict_class(dataset.images[i]);
        if (predicted == dataset.labels[i]) correct++;
    }
    
    double accuracy = 100.0 * correct / dataset.images.size();
    std::cout << "Training Accuracy: " << accuracy << "%" << std::endl;
    
    nn.save(model_file);
    
    std::ofstream class_file("../models/classes.txt");
    for (const auto& name : dataset.class_names) {
        class_file << name << std::endl;
    }
    class_file.close();
    std::cout << "Class names saved to ../models/classes.txt" << std::endl;
    
    std::cout << "\nTraining complete!" << std::endl;
    
    return 0;
}
