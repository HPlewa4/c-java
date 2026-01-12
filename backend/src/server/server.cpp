#include "model/cnn.h"
#include <opencv2/opencv.hpp>
#include <microhttpd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <csignal>
#include <atomic>
#include <condition_variable>
#include <mutex>

ConvolutionalNeuralNetwork* model = nullptr;
std::atomic<bool> keep_running(true);
std::condition_variable shutdown_cv;
std::mutex shutdown_mutex;

void handle_signal(int) {
    if (keep_running.exchange(false)) {
        shutdown_cv.notify_one();
    }
}

std::vector<std::vector<std::vector<double>>> preprocess_image(const cv::Mat& img) {
    cv::Mat resized;
    cv::resize(img, resized, cv::Size(64, 64));

    std::vector<std::vector<std::vector<double>>> image(
        3, std::vector<std::vector<double>>(64, std::vector<double>(64)));

    for (int h = 0; h < 64; h++) {
        for (int w = 0; w < 64; w++) {
            cv::Vec3b pixel = resized.at<cv::Vec3b>(h, w);
            image[0][h][w] = pixel[2] / 255.0;
            image[1][h][w] = pixel[1] / 255.0;
            image[2][h][w] = pixel[0] / 255.0;
        }
    }

    return image;
}

std::string create_json_response(int prediction, double confidence) {
    std::stringstream ss;
    ss << "{";
    ss << "\"prediction\":" << prediction << ",";
    ss << "\"confidence\":" << confidence << ",";
    ss << "\"label\":\"" << (prediction == 1 ? "Receding Hairline" : "No Receding Hairline") << "\"";
    ss << "}";
    return ss.str();
}

struct ConnectionInfo {
    std::string data;
    bool processing = false;
};

static MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                       const char *url, const char *method,
                                       const char *version, const char *upload_data,
                                       size_t *upload_data_size, void **con_cls) {

    if (*con_cls == nullptr) {
        auto *con_info = new ConnectionInfo();
        *con_cls = con_info;
        std::cout << "New connection: " << method << " " << url << std::endl;
        return MHD_YES;
    }

    auto *con_info = static_cast<ConnectionInfo*>(*con_cls);
    std::cout << "Processing: " << method << " " << url << ", upload_data_size=" << *upload_data_size << std::endl;

    if (strcmp(method, "OPTIONS") == 0) {
        std::string response = "";
        auto *resp = MHD_create_response_from_buffer(0, (void*)response.c_str(),
                                                      MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(resp, "Access-Control-Allow-Origin", "*");
        MHD_add_response_header(resp, "Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        MHD_add_response_header(resp, "Access-Control-Allow-Headers", "Content-Type");
        MHD_Result ret = static_cast<MHD_Result>(MHD_queue_response(connection, MHD_HTTP_OK, resp));
        MHD_destroy_response(resp);
        return ret;
    }

    if (strcmp(method, "POST") == 0 && strcmp(url, "/classify") == 0) {
        if (*upload_data_size != 0) {
            std::cout << "Receiving data chunk: " << *upload_data_size << " bytes" << std::endl;
            con_info->data.append(upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        }

        std::cout << "Processing POST /classify with " << con_info->data.size() << " total bytes" << std::endl;

        std::string response;
        int status = MHD_HTTP_OK;

        if (con_info->data.empty()) {
            std::cout << "Error: No image data received" << std::endl;
            response = "{\"error\":\"No image data received\"}";
            status = MHD_HTTP_BAD_REQUEST;
        } else if (model == nullptr) {
            response = "{\"error\":\"Model not loaded\"}";
            status = MHD_HTTP_BAD_REQUEST;
        } else {
            std::cout << "Received " << con_info->data.size() << " bytes of image data" << std::endl;
            std::vector<unsigned char> img_data(con_info->data.begin(), con_info->data.end());
            cv::Mat img = cv::imdecode(img_data, cv::IMREAD_COLOR);

            if (img.empty()) {
                response = "{\"error\":\"Failed to decode image (invalid format or corrupted data)\"}";
                status = MHD_HTTP_BAD_REQUEST;
            } else {
                try {
                    auto processed = preprocess_image(img);
                    double confidence = model->forward(processed);
                    int prediction = model->predict(processed);
                    response = create_json_response(prediction, confidence);
                } catch (const std::exception& e) {
                    response = "{\"error\":\"" + std::string(e.what()) + "\"}";
                    status = MHD_HTTP_INTERNAL_SERVER_ERROR;
                }
            }
        }

        auto *resp = MHD_create_response_from_buffer(response.length(),
                                                      (void*)response.c_str(),
                                                      MHD_RESPMEM_MUST_COPY);
        MHD_add_response_header(resp, "Content-Type", "application/json");
        MHD_add_response_header(resp, "Access-Control-Allow-Origin", "*");
        MHD_Result ret = static_cast<MHD_Result>(MHD_queue_response(connection, status, resp));
        MHD_destroy_response(resp);
        return ret;
    }

    if (strcmp(method, "GET") == 0 && strcmp(url, "/health") == 0) {
        std::string response = "{\"status\":\"healthy\",\"modelLoaded\":"
                             + std::string(model != nullptr ? "true" : "false") + "}";
        auto *resp = MHD_create_response_from_buffer(response.length(),
                                                      (void*)response.c_str(),
                                                      MHD_RESPMEM_MUST_COPY);
        MHD_add_response_header(resp, "Content-Type", "application/json");
        MHD_add_response_header(resp, "Access-Control-Allow-Origin", "*");
        MHD_Result ret = static_cast<MHD_Result>(MHD_queue_response(connection, MHD_HTTP_OK, resp));
        MHD_destroy_response(resp);
        return ret;
    }

    std::string response = "{\"error\":\"Not found\"}";
    auto *resp = MHD_create_response_from_buffer(response.length(),
                                                  (void*)response.c_str(),
                                                  MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(resp, "Access-Control-Allow-Origin", "*");
    MHD_Result ret = static_cast<MHD_Result>(MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, resp));
    MHD_destroy_response(resp);
    return ret;
}

static void request_completed(void *cls, struct MHD_Connection *connection,
                              void **con_cls, enum MHD_RequestTerminationCode toe) {
    auto *con_info = static_cast<ConnectionInfo*>(*con_cls);
    if (con_info != nullptr) delete con_info;
}

int main(int argc, char* argv[]) {
    std::string model_file = "../models/cnn_libtorch_final.pt";
    int port = 8080;

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    if (argc > 1) model_file = argv[1];
    if (argc > 2) port = std::atoi(argv[2]);

    std::cout << "=== Receding Hairline Detection Server ===" << std::endl;
    std::cout << "Loading CNN model from: " << model_file << std::endl;

    model = new ConvolutionalNeuralNetwork();
    try {
        model->load(model_file);
        model->set_training(false);
        std::cout << "✓ Model loaded successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        delete model;
        return 1;
    }

    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY,
                                                 port, NULL, NULL,
                                                 &answer_to_connection, NULL,
                                                 MHD_OPTION_NOTIFY_COMPLETED,
                                                 &request_completed, NULL,
                                                 MHD_OPTION_END);
    if (daemon == NULL) {
        std::cerr << "Failed to start server" << std::endl;
        delete model;
        return 1;
    }

    std::cout << "\n✓ Server running on http://localhost:" << port << std::endl;
    std::cout << "\nEndpoints:" << std::endl;
    std::cout << "  GET  /health   - Health check" << std::endl;
    std::cout << "  POST /classify - Classify image (binary classification)" << std::endl;
    std::cout << "\nPress Ctrl+C to stop...\n" << std::endl;

    {
        std::unique_lock<std::mutex> lock(shutdown_mutex);
        shutdown_cv.wait(lock, [] { return !keep_running.load(); });
    }

    std::cout << "\nShutting down server..." << std::endl;
    MHD_stop_daemon(daemon);

    std::cout << "Cleaning up resources..." << std::endl;
    delete model;

    std::cout << "Server stopped successfully!" << std::endl;
    return 0;
}