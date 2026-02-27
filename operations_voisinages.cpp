#include <iostream>
#include <thread>
#include <chrono>
#include "open_image.h"
#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;

Mat get_filtre_moyenne(const int filtre_p){
    
    const double moyenne = 1.0/pow(filtre_p,2);
    cout << moyenne << endl;

    Mat kernel = Mat::ones(filtre_p, filtre_p, CV_32F) * moyenne;
    cout << kernel << endl;

    return kernel;
}

// In threaded functions do not use cout (or any io operation), it creates a huge bottleneck
void thread_of_convolution(int start_row, int end_row, const Mat* image, const Mat* masque, Mat* new_image){

    const int rows = image->rows;
    const int cols = image->cols;
    const int masque_rows = masque->rows;
    const int masque_cols = masque->cols;
    const int masque_p = (masque->cols - 1)/2;


    for (int r = start_row; r<end_row; r++){
        for (int c = 0; c<cols; c++){

            double sum = 0.0;
            for (int r_temp = 0; r_temp<masque_rows; r_temp++){
                for (int c_temp = 0; c_temp<masque_cols; c_temp++){

                    int row_loc_in_image = r - (r_temp - masque_p);
                    int col_loc_in_image = c - (c_temp - masque_p);

                    if (!(col_loc_in_image < 0 || row_loc_in_image < 0 || col_loc_in_image >= cols || row_loc_in_image >= rows)) {
                     
                        //cout << "here " << r << " - " << c << endl;   
                    
                    
                        sum += masque->at<float>(r_temp, c_temp) * image->at<uchar>(row_loc_in_image, col_loc_in_image);
                    }
                }
            }
            new_image->at<uchar>(r,c) = saturate_cast<uchar>(sum);
        }
    }
}

Mat apply_filtre_convolution(const Mat* image, const Mat* masque, const int thread_count){

    const int rows = image->rows;
    const int cols = image->cols;

    const int masque_rows = masque->rows;
    const int masque_cols = masque->cols;
    const int masque_p = (masque->cols - 1)/2;

    Mat new_image(rows, cols, CV_8UC1, Scalar(0));

    vector<thread> threads;
    const int proportion = rows / thread_count;

    for (int i = 0; i<thread_count; i++){
        int start = proportion * i;
        int end = i == thread_count-1 ? rows : proportion * (i+1);
        cout << "start - end: " << start << " - " << end << endl;

     
        threads.push_back(thread(thread_of_convolution, start, end, image, masque, &new_image));
    }

    for (int k = 0; k<thread_count; k++){
        threads[k].join();
    }
    
    return new_image;
}

// to compile and run: g++ operations_voisinages.cpp open_image.cpp -o operations_voisinages -I/usr/local/include/opencv4 -lopencv_core -lopencv_imgcodecs && ./operations_voisinages

int main(){

    auto start = chrono::high_resolution_clock::now();


    const string file_path = "image1.png";
    const int thread_count = 10;
    const int filtre_p = 3;

    const Mat image = read_image_gray(file_path);

    
    

    Mat kernel = (Mat_<float>(filtre_p, filtre_p) <<
     -1,  0,  1,
     -1, 0,  1,
     -1,  0,  1);


    Mat new_image = apply_filtre_convolution(&image, &kernel, thread_count);


    imwrite("created_images/previt.png", new_image);


    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << " seconds\n";

    return 0;
}

/*
// filtre moyenne
Mat kernel = get_filtre_moyenne(filtre_p);

// filtre laplacienne
Mat kernel = (Mat_<float>(filtre_p, filtre_p) <<
     1,  1,  1,
     1, -8,  1,
     1,  1,  1);



// unsharp masking
const double lambda = 1.0;

Mat kernel = (Mat_<float>(filtre_p, filtre_p) <<
 -lambda,  -lambda,  -lambda,
 -lambda, 1+lambda*8,  -lambda,
 -lambda,  -lambda,  -lambda);

//previt
Mat kernel = (Mat_<float>(filtre_p, filtre_p) <<
     -1,  0,  1,
     -1, 0,  1,
     -1,  0,  1);
*/