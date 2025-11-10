#include "neural_network.h"
#include <opencv2/opencv.hpp>
#include <microhttpd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

std::vector<std::string> class_names;
NeuralNetwork* nn = nullptr;
int IMG_SIZE = 32;

std::vector<double> image_to_vector(const cv::Mat& img) {
    cv::Mat resized, gray;
    cv::resize(img, resized, cv::Size(IMG_SIZE, IMG_SIZE));
    cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
    
    std::vector<double> vec;
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            vec.push_back(gray.at<uchar>(i, j) / 255.0);
        }
    }
    return vec;
}

std::string create_json_response(const std::vector<double>& probs) {
    std::stringstream ss;
    ss << "{\"predictions\":[";
    
    std::vector<std::pair<double, int>> indexed_probs;
    for (size_t i = 0; i < probs.size(); i++) {
        indexed_probs.push_back({probs[i], i});
    }
    std::sort(indexed_probs.begin(), indexed_probs.end(), 
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(size_t(5), indexed_probs.size()); i++) {
        if (i > 0) ss << ",";
        ss << "{\"className\":\"" << class_names[indexed_probs[i].second] << "\","
           << "\"confidence\":" << indexed_probs[i].first << ","
           << "\"classId\":" << indexed_probs[i].second << "}";
    }
    
    ss << "]}";
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
        return MHD_YES;
    }
    
    auto *con_info = static_cast<ConnectionInfo*>(*con_cls);
    
    if (strcmp(method, "POST") == 0 && strcmp(url, "/classify") == 0) {
        if (*upload_data_size != 0) {
            con_info->data.append(upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        }
        
        std::vector<unsigned char> img_data(con_info->data.begin(), con_info->data.end());
        cv::Mat img = cv::imdecode(img_data, cv::IMREAD_COLOR);
        
        std::string response;
        int status = MHD_HTTP_OK;
        
        if (img.empty() || nn == nullptr) {
            response = "{\"error\":\"Failed to process image\"}";
            status = MHD_HTTP_BAD_REQUEST;
        } else {
            auto vec = image_to_vector(img);
            auto probs = nn->forward(vec);
            response = create_json_response(probs);
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
                             + std::string(nn != nullptr ? "true" : "false") + "}";
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
    std::string model_file = "../models/trained_model.bin";
    std::string classes_file = "../models/classes.txt";
    int port = 8080;
    
    if (argc > 1) model_file = argv[1];
    if (argc > 2) classes_file = argv[2];
    if (argc > 3) port = std::atoi(argv[3]);
    
    std::cout << "=== Image Classifier Server ===" << std::endl;
    
    std::ifstream cf(classes_file);
    if (!cf.is_open()) {
        std::cerr << "Could not open classes file: " << classes_file << std::endl;
        return 1;
    }
    
    std::string line;
    while (std::getline(cf, line)) {
        class_names.push_back(line);
    }
    cf.close();
    
    std::cout << "Loaded " << class_names.size() << " classes" << std::endl;
    
    nn = new NeuralNetwork({IMG_SIZE * IMG_SIZE, 128, (int)class_names.size()});
    nn->load(model_file);
    
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY,
                                                 port, NULL, NULL,
                                                 &answer_to_connection, NULL,
                                                 MHD_OPTION_NOTIFY_COMPLETED,
                                                 &request_completed, NULL,
                                                 MHD_OPTION_END);
    if (daemon == NULL) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    std::cout << "Server running on http://localhost:" << port << std::endl;
    std::cout << "Endpoints:" << std::endl;
    std::cout << "  GET  /health   - Health check" << std::endl;
    std::cout << "  POST /classify - Classify image" << std::endl;
    std::cout << "\nPress Enter to stop server and exit..." << std::endl;

    getchar();

    std::cout << "Shutting down server..." << std::endl;
    MHD_stop_daemon(daemon);

    std::cout << "Cleaning up resources..." << std::endl;
    delete nn;

    std::cout << "Server stopped successfully. Goodbye!" << std::endl;
    return 0;
}
