#pragma once

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "read_cifar_images.h"


cv::Mat flatten_type_conversion(std::vector<Dataset>* data);
void mean_substractor(std::vector<Dataset>* data);
void preprocess_data(std::vector<Dataset>* data);