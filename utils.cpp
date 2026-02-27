#include <vector>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "read_cifar_images.h"

using namespace std;
using namespace cv;

// type conversion ->  normalize to [-1,1] -> flatten -> mean substract

Mat flatten_type_conversion(vector<Dataset>* data){
    /*
        Converts images from CV_8UC3 to CV_32FC3, normalizes to [-1,1], flattens from [32 * 32 * 3] to [1 * 3072 * 1], computes mean, in place modification
        Returns the mean
    */

    Mat& sample_image = (*data)[0].image;
    const int image_size = sample_image.total() * sample_image.channels();

    Mat mean = Mat::zeros(1, image_size, CV_32F);

    for (int i = 0; i<data->size(); i++){

        // convert to float
        (*data)[i].image.convertTo((*data)[i].image, CV_32F);
        
        // normalize to [-1,1]
        (*data)[i].image /= 127.5;
        (*data)[i].image -= 1.0;

        // flatten the image
        (*data)[i].image = (*data)[i].image.reshape(1,1); 

        // sum
        mean += (*data)[i].image;
    }
    mean /= (*data).size();

    return mean;
}

void mean_substractor(vector<Dataset>* data){
    /*
        Gets the mean and substracts it from every image

        TODO: Parallelize it with threads
    */

    Mat mean = flatten_type_conversion(data);

    Mat& sample_image = (*data)[0].image;
    const int image_size = sample_image.total() * sample_image.channels();

    // substract the mean from every image
    for (int i = 0; i<(*data).size(); i++){
        (*data)[i].image -= mean;

    }
}

void preprocess_data(vector<Dataset>* data){
    mean_substractor(data);
}