#include "model/cnn.h"
#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
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

std::vector<std::vector<std::vector<double>>> load_image_cpu(const std::string& full_path) {
    cv::Mat img = cv::imread(full_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + full_path);
    }

    cv::resize(img, img, cv::Size(64, 64));

    std::vector<std::vector<std::vector<double>>> image(
        3, std::vector<std::vector<double>>(64, std::vector<double>(64)));

    for (int h = 0; h < 64; h++) {
        for (int w = 0; w < 64; w++) {
            cv::Vec3b pixel = img.at<cv::Vec3b>(h, w);
            image[0][h][w] = pixel[2] / 255.0;
            image[1][h][w] = pixel[1] / 255.0;
            image[2][h][w] = pixel[0] / 255.0;
        }
    }

    return image;
}

torch::Tensor load_image_gpu(const std::string& full_path) {
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

void print_metrics(int correct, int total, int tp, int fp, int tn, int fn) {
    double accuracy = (double)correct / total * 100.0;
    double precision = (tp + fp > 0) ? (double)tp / (tp + fp) * 100.0 : 0.0;
    double recall = (tp + fn > 0) ? (double)tp / (tp + fn) * 100.0 : 0.0;
    double f1_score = (precision + recall > 0) ? 2.0 * (precision * recall) / (precision + recall) : 0.0;

    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total samples:        " << total << std::endl;
    std::cout << "Correct predictions:  " << correct << std::endl;
    std::cout << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Overall Accuracy:     " << accuracy << "%" << std::endl;
    std::cout << "Precision:            " << precision << "%" << std::endl;
    std::cout << "Recall:               " << recall << "%" << std::endl;
    std::cout << "F1 Score:             " << f1_score << "%" << std::endl;
    std::cout << std::endl;
    std::cout << "Confusion Matrix:" << std::endl;
    std::cout << "  True Positives:     " << tp << std::endl;
    std::cout << "  True Negatives:     " << tn << std::endl;
    std::cout << "  False Positives:    " << fp << std::endl;
    std::cout << "  False Negatives:    " << fn << std::endl;
    std::cout << "========================================" << std::endl;
}

void test_cpu_model(const std::string& model_path,
                    const std::vector<CelebASample>& samples,
                    const std::string& base_path) {
    std::cout << "Loading CPU model..." << std::endl;
    ConvolutionalNeuralNetwork model;
    model.load(model_path);
    model.set_training(false);
    std::cout << "Model loaded successfully!" << std::endl;
    std::cout << std::endl;

    std::cout << "Testing on " << samples.size() << " samples..." << std::endl;
    std::cout << std::endl;

    int correct = 0, total = 0;
    int tp = 0, fp = 0, tn = 0, fn = 0;

    std::cout << "First 10 predictions:" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < samples.size(); i++) {
        const auto& sample = samples[i];
        std::string full_path = base_path + "/" + sample.path;

        try {
            auto image = load_image_cpu(full_path);
            double prob = model.forward(image);
            int predicted = model.predict(image);

            if (predicted == sample.label) correct++;

            if (sample.label == 1 && predicted == 1) tp++;
            else if (sample.label == 0 && predicted == 1) fp++;
            else if (sample.label == 0 && predicted == 0) tn++;
            else if (sample.label == 1 && predicted == 0) fn++;

            total++;

            if (i < 10) {
                std::string true_label = (sample.label == 1) ? "    Receding" : " No Receding";
                std::string pred_label = (predicted == 1) ? "    Receding" : " No Receding";
                std::string match = (predicted == sample.label) ? "✓" : "✗";
                printf("%3zu. %s -> %s (%.3f) %s\n",
                       i + 1, true_label.c_str(), pred_label.c_str(), prob, match.c_str());
            }

            if ((i + 1) % 100 == 0) {
                std::cout << "\r  Progress: " << (i + 1) << "/" << samples.size();
                std::cout.flush();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    std::cout << "\r                                        \r";
    std::cout << "------------------------------------------------------------" << std::endl;

    print_metrics(correct, total, tp, fp, tn, fn);
}

struct CNNImpl : torch::nn::Module {
    torch::nn::Conv2d conv1{nullptr}, conv2{nullptr}, conv3{nullptr}, conv4{nullptr};
    torch::nn::MaxPool2d pool1{nullptr}, pool2{nullptr}, pool3{nullptr};
    torch::nn::Dropout2d dropout1{nullptr}, dropout2{nullptr};
    torch::nn::AvgPool2d avgpool{nullptr};
    torch::nn::Linear fc{nullptr};

    CNNImpl() {
        conv1 = register_module("conv1", torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 32, 3).padding(1)));
        pool1 = register_module("pool1", torch::nn::MaxPool2d(2));
        dropout1 = register_module("dropout1", torch::nn::Dropout2d(0.5));

        conv2 = register_module("conv2", torch::nn::Conv2d(torch::nn::Conv2dOptions(32, 64, 3).padding(1)));
        pool2 = register_module("pool2", torch::nn::MaxPool2d(2));
        dropout2 = register_module("dropout2", torch::nn::Dropout2d(0.5));

        conv3 = register_module("conv3", torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).padding(1)));
        pool3 = register_module("pool3", torch::nn::MaxPool2d(2));

        conv4 = register_module("conv4", torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 256, 3).padding(1)));
        avgpool = register_module("avgpool", torch::nn::AvgPool2d(8));

        fc = register_module("fc", torch::nn::Linear(256, 1));
    }

    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(conv1->forward(x));
        x = pool1->forward(x);
        x = dropout1->forward(x);

        x = torch::relu(conv2->forward(x));
        x = pool2->forward(x);
        x = dropout2->forward(x);

        x = torch::relu(conv3->forward(x));
        x = pool3->forward(x);

        x = torch::relu(conv4->forward(x));
        x = avgpool->forward(x);

        x = x.view({x.size(0), -1});
        x = torch::sigmoid(fc->forward(x));

        return x;
    }
};
TORCH_MODULE(CNN);

void test_gpu_model(const std::string& model_path,
                    const std::vector<CelebASample>& samples,
                    const std::string& base_path) {
    std::cout << "Loading GPU model..." << std::endl;

    torch::Device device(torch::kCPU);
    if (torch::cuda::is_available()) {
        device = torch::Device(torch::kCUDA);
        std::cout << "Using CUDA GPU" << std::endl;
    } else if (torch::mps::is_available()) {
        device = torch::Device(torch::kMPS);
        std::cout << "Using Apple Metal GPU" << std::endl;
    }

    CNN model;
    torch::load(model, model_path);
    model->to(device);
    model->eval();
    std::cout << "Model loaded successfully!" << std::endl;
    std::cout << std::endl;

    std::cout << "Testing on " << samples.size() << " samples..." << std::endl;
    std::cout << std::endl;

    int correct = 0, total = 0;
    int tp = 0, fp = 0, tn = 0, fn = 0;

    std::cout << "First 10 predictions:" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    torch::NoGradGuard no_grad;

    for (size_t i = 0; i < samples.size(); i++) {
        const auto& sample = samples[i];
        std::string full_path = base_path + "/" + sample.path;

        try {
            auto img = load_image_gpu(full_path).unsqueeze(0).to(device);
            auto output = model->forward(img);
            float prob = output.item<float>();
            int predicted = (prob >= 0.5) ? 1 : 0;

            if (predicted == sample.label) correct++;

            if (sample.label == 1 && predicted == 1) tp++;
            else if (sample.label == 0 && predicted == 1) fp++;
            else if (sample.label == 0 && predicted == 0) tn++;
            else if (sample.label == 1 && predicted == 0) fn++;

            total++;

            if (i < 10) {
                std::string true_label = (sample.label == 1) ? "    Receding" : " No Receding";
                std::string pred_label = (predicted == 1) ? "    Receding" : " No Receding";
                std::string match = (predicted == sample.label) ? "✓" : "✗";
                printf("%3zu. %s -> %s (%.3f) %s\n",
                       i + 1, true_label.c_str(), pred_label.c_str(), prob, match.c_str());
            }

            if ((i + 1) % 100 == 0) {
                std::cout << "\r  Progress: " << (i + 1) << "/" << samples.size();
                std::cout.flush();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    std::cout << "\r                                        \r";
    std::cout << "------------------------------------------------------------" << std::endl;

    print_metrics(correct, total, tp, fp, tn, fn);
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Unified Model Testing" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::string model_path;
    std::string model_type = "auto";

    if (argc > 1) {
        model_path = argv[1];
    }

    if (argc > 2) {
        std::string arg = argv[2];
        if (arg == "--type") {
            if (argc > 3) {
                model_type = argv[3];
            }
        }
    }

    if (model_type == "auto") {
        if (model_path.find(".pt") != std::string::npos) {
            model_type = "gpu";
        } else if (model_path.find(".bin") != std::string::npos) {
            model_type = "cpu";
        } else {
            std::cout << "Usage: " << argv[0] << " <model_path> [--type cpu|gpu]" << std::endl;
            std::cout << "Model type will be auto-detected from extension (.pt = GPU, .bin = CPU)" << std::endl;
            return 1;
        }
    }

    if (model_path.empty()) {
        if (model_type == "cpu") {
            model_path = "../models/cnn_cpu.bin";
        } else {
            model_path = "../models/cnn_gpu.pt";
        }
    }

    std::string base_path = "../src/dataset";
    std::string test_split = base_path + "/data/celeba_splits/test.txt";

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Model path: " << model_path << std::endl;
    std::cout << "  Model type: " << model_type << std::endl;
    std::cout << "  Test split: " << test_split << std::endl;
    std::cout << std::endl;

    std::cout << "Loading test data..." << std::endl;
    auto test_samples = load_sample_list(test_split);
    std::cout << "  Loaded " << test_samples.size() << " test samples" << std::endl;
    std::cout << std::endl;

    try {
        if (model_type == "cpu") {
            test_cpu_model(model_path, test_samples, base_path);
        } else if (model_type == "gpu") {
            test_gpu_model(model_path, test_samples, base_path);
        } else {
            std::cerr << "Unknown model type: " << model_type << std::endl;
            std::cerr << "Use --type cpu or --type gpu" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error testing model: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}