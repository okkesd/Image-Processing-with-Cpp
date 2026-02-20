#include <stdlib.h>
#include <vector>
#include <cstdint>
#include <sstream>
#include <numeric>
#include <string>
#include <cassert>
#include <thread>
//#include "utils.h"
#include "open_image.h"
//#include "opencv2/opencv.hpp"
#include <chrono>


using namespace std;
using namespace cv;

void increase_constrast(Mat* image){
    /*
        function to increase contrast of a given image
        in place modification
    */

    const int rows = image->rows;
    const int cols = image->cols;

    for (int r = 0; r<rows; r++){
        for (int c = 0; c<cols; c++){

            if ((int)image->at<uchar>(r,c) < 80){

                image->at<uchar>(r,c) = (int)(((double)image->at<uchar>(r,c))*0.6);

            } else if (80 <= (int)image->at<uchar>(r,c) < 128){

                image->at<uchar>(r,c) = (int)(((double)image->at<uchar>(r,c))*1.68) - 86;
            }
            else if (128 <= (int)image->at<uchar>(r,c) < 184){

                image->at<uchar>(r,c) = (int)(((double)image->at<uchar>(r,c))*1.28) - 35;
            } else {

                image->at<uchar>(r,c) = (int)(((double)image->at<uchar>(r,c))*0.77) + 59;
            }
        }
    }
}

void negative_image(Mat* image){
    /*
        function to make the image negative
        in place modification
    */

    const int rows = image->rows;
    const int cols = image->cols;

    for (int r = 0; r<rows; r++){
        for (int c = 0; c<cols; c++){

            image->at<uchar>(r,c) = 255 - image->at<uchar>(r,c);
        }
    }
}

void slice_interval(Mat* image){
    /*
        function to select an interval, and only display it at a certain pixel value (certain color)
        in place modification
    */

    const int rows = image->rows;
    const int cols = image->cols;

    const int threshold_start = 60;
    const int threshold_end = 199;

    for (int r = 0; r<rows; r++){
        for (int c = 0; c<cols; c++){

            if (threshold_start < (int)image->at<uchar>(r,c) && (int)image->at<uchar>(r,c) < threshold_end){
                
                image->at<uchar>(r,c) = 144;
            } else {

                image->at<uchar>(r,c) = 0;
            }
        }
    }
}


void slice_threshold(Mat* image){
    /*
        function to select the pixel that are higher than a certain threshold, they're displayed at certain pixel value
        in place modification
    */

    const int rows = image->rows;
    const int cols = image->cols;

    const int threshold = 60;

    for (int r = 0; r<rows; r++){
        for (int c = 0; c<cols; c++){

            if (threshold < (int)image->at<uchar>(r,c)){
                
                image->at<uchar>(r,c) = 144;
            } else {

                image->at<uchar>(r,c) = 0;
            }
        }
    }
}

void get_histogram(Mat* image, vector<double>* values_counts){

    const int rows = image->rows;
    const int cols = image->cols;
    const double total = static_cast<double>(image->total());

    //vector<int> values_counts(255, 0);

    for (int r = 0; r<rows; r++){
        for (int c = 0; c<cols; c++){

            const int value = (int)image->at<uchar>(r,c);
            (*values_counts)[value] += 1;

        }
    }

    // int max_value = *std::max_element((*values_counts).begin(), (*values_counts).end());

    for (int i = 0; i<(*values_counts).size(); i++){
        (*values_counts)[i] /= total;
    }
}

// to run and compile: g++ ./operations_ponctuelles.cpp ./open_image.cpp -I/usr/local/include/opencv4 -lopencv_core -lopencv_imgcodecs -o operations_ponctuelles && ./operations_ponctuelles

int main(){
    string file_path = "image1.png";

    Mat image = read_image_gray(file_path);


    // increase_constrast(&image);

    // slice_interval(&image);

    // slice_threshold(&image);

    //vector<double>* histogram = &vector<double>(255, 0);
    vector<double> histogram(256, 0);
    get_histogram(&image, &histogram);

    double sum = 0.0;
    for (int i = 0; i<histogram.size(); i++){
        cout << histogram[i] << endl;
        sum += histogram[i];
    }

    cout << "sum is: " << sum << endl;

    // imwrite("gray_image_contrasted.png", image);

    return 0;
}