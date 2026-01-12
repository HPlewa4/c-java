#ifndef AUGMENTATION_H
#define AUGMENTATION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <random>

class ImageAugmentation {
private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> uniform_dist;

public:
    ImageAugmentation();
    cv::Mat random_crop(const cv::Mat& img, int crop_size);
    cv::Mat center_crop(const cv::Mat& img, int crop_size);
    cv::Mat random_horizontal_flip(const cv::Mat& img);

    std::vector<std::vector<std::vector<double>>> training_transform(
        const cv::Mat& img,
        int target_size = 64);

    std::vector<std::vector<std::vector<double>>> validation_transform(
        const cv::Mat& img,
        int target_size = 64);

private:
    std::vector<std::vector<std::vector<double>>> mat_to_tensor(const cv::Mat& img);
};

#endif