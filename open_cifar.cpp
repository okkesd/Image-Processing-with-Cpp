#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <vector>

#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
//#include <opencv2/core.hpp>
//#include <opencv2/core/utility.hpp>
//#include "opencv2/imgcodecs.hpp"

using namespace std;
/*

ifstream rf(bin_path, ios::binary);
    if (rf.is_open()){
        cout << "read" << endl;

        /*rf.seekg(0, ios::end);
        size_t size = rf.tellg();
        rf.seekg(0, ios::beg);

        // read the label at first index
        unsigned char label;
        rf.read((char*)&label, 1);

        int image_size = 32 * 32;

        vector<unsigned char> buffer(3 * image_size);
        rf.read((char*)buffer.data(), buffer.size());

        for (int i = 0; i<10; i++){
            cout << (int)buffer[i] << endl;
        }
    }

*/

using namespace cv;

// to compile and run: g++ open_cifar.cpp -o open_cifar -I/usr/local/include/opencv4 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui  && ./open_cifar

int main(){
    string bin_path = "/home/debianokkes/Downloads/cifar-10-batches-bin/data_batch_1.bin";

    ifstream file(bin_path, ios::binary);
    if (!file){
        return 1;
    }

    unsigned char label;
    file.read((char*)&label, 1);

    // image size is already defined in cifar: 32 * 32
    const int image_size = 32 * 32;

    vector<unsigned char> buffer(3 * image_size); // read rgb

    // read as the size of the buffer which is just the size of the image
    file.read((char*)buffer.data(), buffer.size()); 

    Mat image(32, 32, CV_8UC3);

    for (int i = 0; i<image_size; i++){

        int row = i / 32;
        int col = i % 32;

        image.at<Vec3b>(row, col)[2] = buffer[i];
        image.at<Vec3b>(row, col)[1] = buffer[i + image_size]; // go forward in the flat array as the size of the image to reach the next color
        image.at<Vec3b>(row, col)[0] = buffer[i + 2*image_size];
    }

    cout << "label is: " << (int)label << endl;

    for (int r = 0; r<10; r++){
        for (int c = 0; c<10; c++){
            cout << (int)image.at<uchar>(r,c) << " ";
        }
        cout << endl;
    }

    imwrite("new_cifar.png", image);
    //waitkey(0);

    
    return 0;
}