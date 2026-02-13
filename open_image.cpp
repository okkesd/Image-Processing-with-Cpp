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

// to compile and run: g++ open_image.cpp -o open_image && ./open_image
int main(){
    string file_name = "image1.png";
    //ifstream image(file_name); // , ios::in
    Mat image = cv::imread(file_name, IMREAD_GRAYSCALE);
    cout << "read"<< endl;
    if (!image.empty()){

        for (int r = 600; r<700; r++){
            uchar* row = image.ptr<uchar>(r);
            for (int c = 600; c<700; c++){

                cout << (int)image.at<uchar>(r,c) << " ";
                if (image.at<uchar>(r,c) > 205){
                    image.at<uchar>(r,c) = 50;
                } else {
                    image.at<uchar>(r,c) += 50;
                }
                
                //cout << (int)row[c] << " ";
            }
            cout << endl;
        }

        imwrite("out.png", image);
        
        
        
        //image.close();
    } else {

    }
    
    return 0;
}