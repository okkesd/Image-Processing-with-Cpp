#include <stdlib.h>
#include <fstream>

#include <iostream>
#include <sstream>
//#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace std;
using namespace cv;
//using namespace fstream;

Mat read_image_gray(string file_path){
    /*
        Reads a given image with opencv and returns a Mat
    */

    Mat image = imread(file_path, IMREAD_GRAYSCALE);
    if (image.empty()){
        cout << "Image read failed in read_image_gray" << endl;
        return Mat();
    }
    cout << "Image read" << endl;
    
    return image;
}

// to compile and run: g++ open_image.cpp -o open_image -I/usr/local/include/opencv4 -lopencv_core -lopencv_imgcodecs && ./open_image

int main(){
    string file_name = "image1.png";
    
    // Read image as gray
    Mat image = read_image_gray(file_name);
    
    if (!image.empty()){

        // save gray image
        imwrite("out.png", image);
    } else {

        cout << "Image read failed" << endl;
        exit(1);
    }
    
    return 0;
}