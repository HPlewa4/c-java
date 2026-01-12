#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

struct CNNImpl : torch::nn::Module {
    torch::nn::Conv2d conv1{nullptr}, conv2{nullptr}, conv3{nullptr}, conv4{nullptr};
    torch::nn::MaxPool2d pool1{nullptr}, pool2{nullptr}, pool3{nullptr};
    torch::nn::Dropout2d dropout1{nullptr}, dropout2{nullptr};
    torch::nn::AvgPool2d avgpool{nullptr};
    torch::nn::Linear fc{nullptr};

    CNNImpl() {
        conv1 = register_module("conv1", torch::nn::Conv2d(
            torch::nn::Conv2dOptions(3, 32, 3).padding(1)));
        pool1 = register_module("pool1", torch::nn::MaxPool2d(2));
        dropout1 = register_module("dropout1", torch::nn::Dropout2d(0.5));

        conv2 = register_module("conv2", torch::nn::Conv2d(
            torch::nn::Conv2dOptions(32, 64, 3).padding(1)));
        pool2 = register_module("pool2", torch::nn::MaxPool2d(2));
        dropout2 = register_module("dropout2", torch::nn::Dropout2d(0.5));

        conv3 = register_module("conv3", torch::nn::Conv2d(
            torch::nn::Conv2dOptions(64, 128, 3).padding(1)));
        pool3 = register_module("pool3", torch::nn::MaxPool2d(2));

        conv4 = register_module("conv4", torch::nn::Conv2d(
            torch::nn::Conv2dOptions(128, 256, 3).padding(1)));
        avgpool = register_module("avgpool", torch::nn::AvgPool2d(8));

        fc = register_module("fc", torch::nn::Linear(256, 1));
    }

    torch::Tensor forward(torch::Tensor x) {
        x = conv1->forward(x);
        x = torch::relu(x);
        x = pool1->forward(x);
        x = dropout1->forward(x);

        x = conv2->forward(x);
        x = torch::relu(x);
        x = pool2->forward(x);
        x = dropout2->forward(x);

        x = conv3->forward(x);
        x = torch::relu(x);
        x = pool3->forward(x);

        x = conv4->forward(x);
        x = torch::relu(x);
        x = avgpool->forward(x);

        x = x.view({x.size(0), -1});
        x = fc->forward(x);
        x = torch::sigmoid(x);

        return x;
    }
};
TORCH_MODULE(CNN);

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

torch::Tensor load_image_train(const std::string& full_path) {
    cv::Mat img = cv::imread(full_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + full_path);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist_h(0, img.rows - 178);
    std::uniform_int_distribution<> dist_w(0, img.cols - 178);

    int crop_y = dist_h(gen);
    int crop_x = dist_w(gen);
    cv::Rect crop_rect(crop_x, crop_y, 178, 178);
    img = img(crop_rect);

    std::uniform_real_distribution<> flip_dist(0.0, 1.0);
    if (flip_dist(gen) > 0.5) {
        cv::flip(img, img, 1);
    }

    cv::resize(img, img, cv::Size(64, 64));

    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    img.convertTo(img, CV_32F, 1.0 / 255.0);

    auto tensor = torch::from_blob(img.data, {64, 64, 3}, torch::kFloat32);
    tensor = tensor.permute({2, 0, 1});

    return tensor.clone();
}

torch::Tensor load_image_val(const std::string& full_path) {
    cv::Mat img = cv::imread(full_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + full_path);
    }

    int crop_y = (img.rows - 178) / 2;
    int crop_x = (img.cols - 178) / 2;
    cv::Rect crop_rect(crop_x, crop_y, 178, 178);
    img = img(crop_rect);

    cv::resize(img, img, cv::Size(64, 64));

    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    img.convertTo(img, CV_32F, 1.0 / 255.0);

    auto tensor = torch::from_blob(img.data, {64, 64, 3}, torch::kFloat32);
    tensor = tensor.permute({2, 0, 1});

    return tensor.clone();
}

double evaluate(CNN& model, const std::vector<CelebASample>& samples,
                const std::string& base_path, torch::Device device) {
    model->eval();
    int correct = 0;

    torch::NoGradGuard no_grad;

    for (const auto& sample : samples) {
        std::string full_path = base_path + "/" + sample.path;

        try {
            auto img = load_image_val(full_path).unsqueeze(0).to(device);
            auto output = model->forward(img);
            int pred = (output.item<float>() >= 0.5) ? 1 : 0;

            if (pred == sample.label) {
                correct++;
            }
        } catch (const std::exception& e) {
            continue;
        }
    }

    return 100.0 * correct / samples.size();
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  GPU CNN Training (LibTorch)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::string base_path = "../src/dataset";
    std::string train_split = base_path + "/data/celeba_splits/train.txt";
    std::string val_split = base_path + "/data/celeba_splits/val.txt";
    std::string model_file = "../models/cnn_gpu.pt";

    int epochs = 30;
    double learning_rate = 0.001;
    int batch_size = 32;

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Epochs: " << epochs << std::endl;
    std::cout << "  Learning rate: " << learning_rate << std::endl;
    std::cout << "  Batch size: " << batch_size << std::endl;
    std::cout << "  Optimizer: Adam" << std::endl;
    std::cout << std::endl;

    torch::Device device(torch::kCPU);
    if (torch::cuda::is_available()) {
        device = torch::Device(torch::kCUDA);
        std::cout << "Device: CUDA GPU" << std::endl;
    } else if (torch::mps::is_available()) {
        device = torch::Device(torch::kMPS);
        std::cout << "Device: Apple Metal GPU (MPS)" << std::endl;
    } else {
        std::cout << "Device: CPU" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Loading datasets..." << std::endl;
    auto train_samples = load_sample_list(train_split);
    auto val_samples = load_sample_list(val_split);
    std::cout << "  Training samples: " << train_samples.size() << std::endl;
    std::cout << "  Validation samples: " << val_samples.size() << std::endl;
    std::cout << std::endl;

    CNN model;
    model->to(device);

    int64_t total_params = 0;
    for (const auto& p : model->parameters()) {
        total_params += p.numel();
    }
    std::cout << "Model: 4-Layer CNN with " << total_params << " parameters" << std::endl;
    std::cout << std::endl;

    torch::optim::Adam optimizer(model->parameters(),
                                  torch::optim::AdamOptions(learning_rate));
    auto criterion = torch::nn::BCELoss();

    std::cout << "Starting training..." << std::endl;
    std::cout << "========================================" << std::endl;
    auto training_start = std::chrono::high_resolution_clock::now();

    for (int epoch = 0; epoch < epochs; epoch++) {
        model->train();
        auto epoch_start = std::chrono::high_resolution_clock::now();

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(train_samples.begin(), train_samples.end(), g);

        int processed_samples = 0;

        for (size_t i = 0; i < train_samples.size(); i += batch_size) {
            size_t end = std::min(i + batch_size, train_samples.size());
            size_t current_batch_size = end - i;

            std::vector<torch::Tensor> batch_images;
            std::vector<float> batch_labels;

            for (size_t j = 0; j < current_batch_size; j++) {
                const auto& sample = train_samples[i + j];
                std::string full_path = base_path + "/" + sample.path;

                try {
                    auto img = load_image_train(full_path);
                    batch_images.push_back(img);
                    batch_labels.push_back(static_cast<float>(sample.label));
                } catch (const std::exception& e) {
                    continue;
                }
            }

            if (batch_images.empty()) continue;

            auto images = torch::stack(batch_images).to(device);
            auto labels = torch::tensor(batch_labels).to(device);

            auto output = model->forward(images).squeeze();
            auto loss = criterion(output, labels);

            optimizer.zero_grad();
            loss.backward();
            optimizer.step();

            processed_samples += batch_images.size();

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

        double val_acc = evaluate(model, val_samples, base_path, device);
        std::cout << "    Validation Accuracy: " << std::fixed << std::setprecision(2)
                  << val_acc << "%" << std::endl;

        if ((epoch + 1) % 5 == 0) {
            torch::save(model, model_file);
        }
    }

    auto training_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::minutes>(
        training_end - training_start).count();

    std::cout << "========================================" << std::endl;
    std::cout << "Training completed in " << total_duration << " minutes" << std::endl;

    torch::save(model, model_file);
    std::cout << "Model saved to " << model_file << std::endl;

    return 0;
}