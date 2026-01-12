#include "../../include/utils/augmentation.h"
#include <ctime>

ImageAugmentation::ImageAugmentation()
    : uniform_dist(0.0, 1.0) {
    rng.seed(static_cast<unsigned int>(std::time(nullptr) + std::rand()));
}

cv::Mat ImageAugmentation::random_crop(const cv::Mat& img, int crop_size) {
    int img_height = img.rows;
    int img_width = img.cols;

    int max_y = img_height - crop_size;
    int max_x = img_width - crop_size;

    int start_y = max_y > 0 ? static_cast<int>(uniform_dist(rng) * max_y) : 0;
    int start_x = max_x > 0 ? static_cast<int>(uniform_dist(rng) * max_x) : 0;

    cv::Rect crop_rect(start_x, start_y, crop_size, crop_size);
    return img(crop_rect).clone();
}

cv::Mat ImageAugmentation::center_crop(const cv::Mat& img, int crop_size) {
    int img_height = img.rows;
    int img_width = img.cols;

    int start_y = (img_height - crop_size) / 2;
    int start_x = (img_width - crop_size) / 2;

    start_y = std::max(0, start_y);
    start_x = std::max(0, start_x);

    cv::Rect crop_rect(start_x, start_y, crop_size, crop_size);
    return img(crop_rect).clone();
}

cv::Mat ImageAugmentation::random_horizontal_flip(const cv::Mat& img) {
    if (uniform_dist(rng) < 0.5) {
        cv::Mat flipped;
        cv::flip(img, flipped, 1);
        return flipped;
    }
    return img.clone();
}

std::vector<std::vector<std::vector<double>>> ImageAugmentation::mat_to_tensor(
    const cv::Mat& img) {
    int height = img.rows;
    int width = img.cols;

    std::vector<std::vector<std::vector<double>>> tensor(
        3,
        std::vector<std::vector<double>>(
            height,
            std::vector<double>(width, 0.0)));

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            cv::Vec3b pixel = img.at<cv::Vec3b>(h, w);
            tensor[0][h][w] = pixel[2] / 255.0;
            tensor[1][h][w] = pixel[1] / 255.0;
            tensor[2][h][w] = pixel[0] / 255.0;
        }
    }

    return tensor;
}

std::vector<std::vector<std::vector<double>>> ImageAugmentation::training_transform(
    const cv::Mat& img,
    int target_size) {

    cv::Mat cropped = random_crop(img, 178);

    cv::Mat flipped = random_horizontal_flip(cropped);

    cv::Mat resized;
    cv::resize(flipped, resized, cv::Size(target_size, target_size));

    return mat_to_tensor(resized);
}

std::vector<std::vector<std::vector<double>>> ImageAugmentation::validation_transform(
    const cv::Mat& img,
    int target_size) {

    cv::Mat cropped = center_crop(img, 178);

    cv::Mat resized;
    cv::resize(cropped, resized, cv::Size(target_size, target_size));

    return mat_to_tensor(resized);
}