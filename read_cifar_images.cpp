#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <vector>

#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
//#include "opencv2/highgui.hpp"
//#include <opencv2/core.hpp>
//#include <opencv2/core/utility.hpp>
//#include "opencv2/imgcodecs.hpp"

using namespace std;
using namespace cv;


struct Dataset{
    int label;
    Mat image;

    Dataset(int label, Mat image){
        this->label = label;
        this->image = image;
    };
};

Dataset read_specific_cifar_image(string file_path, int image_order){
    /*
        Reading specific image from .bin file
        image_order: starts from 0, indicates the order of the image in the file we want to read
    */

    ifstream file(file_path, ios::binary);
    if (!file){
        cout << "Image read failed in read_specific_cifar_image" << endl;
        exit(1);
    }
    cout << "Image read" << endl;

    // image size is already defined in cifar: 32 * 32
    const int image_size = 32 * 32;


    for (int i = 0; i<image_order; i++){
        
        // define the empty buffer as the size of the image and its label, we'll read it to move forward to reach the image we want    
        vector<unsigned char> pass_buffer(1 + 3 * image_size);
        unsigned char pass_label;

        file.read((char*)&pass_label, 1);
        file.read((char*)pass_buffer.data(), pass_buffer.size());
    }

    // read the label
    unsigned char label;
    file.read((char*)&label, 1);

    // buffer for reading rgb
    vector<unsigned char> buffer(3 * image_size); 

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

    return Dataset((int)label, image);
}


vector<Dataset> read_cifar_bin(string file_path){
    /*
        Reads cifar binary files, and returns a vector of Dataset defined above
    */

    ifstream images(file_path, ios::binary);

    if (!images){
        cout << "Image read failed in read_cifar_bin" << endl;
        exit(1);
    }
    cout << "Image read" << endl;

    const int image_size = 32 * 32;
    vector<Dataset> data_set;

    while (true) {

        if (!images){
            break;
        }

        // read label
        unsigned char label;
        images.read((char*)&label, 1);
    
        // read image data as the size of image (rgb)
        vector<unsigned char> buffer(3 * image_size);
        images.read((char*)buffer.data(), buffer.size());

        Mat image(32, 32, CV_8UC3);

        for (int i = 0; i<image_size; i++){
            
            int row = i/32;
            int col = i%32;

            image.at<Vec3b>(row,col)[2] = buffer[i];
            image.at<Vec3b>(row,col)[1] = buffer[i + image_size]; // skip to the green
            image.at<Vec3b>(row,col)[0] = buffer[i + 2*image_size]; // skip to the blue
        }


        data_set.push_back(Dataset((int)label, image));
    }
    
    return data_set;
}

// to compile and run: g++ read_cifar_images.cpp -o read_cifar_images -I/usr/local/include/opencv4 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui  && ./read_cifar_images

int main(){
    string bin_path = "/home/debianokkes/Downloads/cifar-10-batches-bin/data_batch_1.bin";

    Dataset image_obj = read_specific_cifar_image(bin_path, 0);

    imwrite("first_image.png", image_obj.image);

    vector<Dataset> data = read_cifar_bin(bin_path);

    cout << data.size() << endl;
    imwrite("five_hundert_from_dataset.png", data[500].image);
    return 0;
}