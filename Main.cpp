#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cctype>

// Base64 decode function
std::vector<uchar> base64_decode(const std::string &encoded_string) {
    static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    uchar char_array_4[4], char_array_3[3];
    std::vector<uchar> ret;

    while (in_len-- && (isalnum(encoded_string[in_]) || encoded_string[in_] == '+' || encoded_string[in_] == '/')) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++) char_array_4[i] = base64_chars.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; i < 3; i++) ret.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++) char_array_4[j] = 0;
        for (j = 0; j <4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);
        char_array_3[0] = ( char_array_4[0] << 2 ) + ( (char_array_4[1] & 0x30) >> 4 );
        char_array_3[1] = ( (char_array_4[1] & 0xf) << 4 ) + ( (char_array_4[2] & 0x3c) >> 2 );
        char_array_3[2] = ( (char_array_4[2] & 0x3) << 6 ) + char_array_4[3];
        for (j = 0; j < i - 1; j++) ret.push_back(char_array_3[j]);
    }

    return ret;
}

int main() {
    std::string base64Image;
    std::cout << "Paste Base64 PNG string:\n";

    // Read full Base64 string (multi-line support)
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        base64Image += line;
    }

    std::vector<uchar> imgData = base64_decode(base64Image);

    cv::Mat img = cv::imdecode(imgData, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        std::cerr << "Failed to decode image!" << std::endl;
        return 1;
    }

    cv::imwrite("output.png", img);
    std::cout << "Image saved as output.png" << std::endl;

    return 0;
}
