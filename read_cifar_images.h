#pragma once

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;

struct Dataset {
    int label;
    cv::Mat image;

    Dataset(int label, const cv::Mat& image)
        : label(label), image(image) {}
};

Dataset read_specific_cifar_image(string file_path, int image_order);

vector<Dataset> read_cifar_bin(string file_path);