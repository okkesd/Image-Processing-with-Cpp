#include <stdlib.h>
#include <vector>
#include <sstream>
#include <numeric>
#include <string>
#include <cassert>
#include <thread>
#include "utils.h"
#include "read_cifar_images.h"
#include "opencv2/opencv.hpp"
#include <chrono>


using namespace std;
using namespace cv;

class NearestNeighbour{
    private: 
        vector<Dataset> data;
        vector<pair<int, vector<int>>> mapped_data;
        int image_pixel_count = 1;

        int ProcessData(){

            // push the first one to iterate on later programmatically
            int label = data[0].label;
            vector<int> first_vec;
            first_vec.push_back(0);
            mapped_data.push_back(make_pair(label, first_vec));

            // iterate over the data and map the indices to labels
            for (int i = 1; i<data.size(); i++){

                int label = data[i].label;
                if (mapped_data.size() > 20) { // defensive check in case of bug, (in dataset, we don't expect more than 10 class)
                    cout << "bug detected" << endl;
                    break;
                }

                int found = 0;
                for (int l = 0; l<mapped_data.size(); l++){

                    if (mapped_data[l].first == label) { // already have, just push the index to correct vector

                        mapped_data[l].second.push_back(i);
                        found = 1;
                        break;
                    } 
                }

                if (!found){ // create a new pair
                        
                    vector<int> indices;
                    indices.push_back(i);

                    mapped_data.push_back(make_pair(label, indices));
                }
            }

            // return 1 as true in case there is no error (checking success as size of mapped data is not best practice!)
            cout << mapped_data.size() << " element" << endl;
            return mapped_data.size() > 0 ?  1 : 0;
        }

        double compute_L1_distance(const Mat& first, const Mat& second) const {

            second.reshape(1,1);
            assert(first.rows == second.rows);
            assert(first.cols == second.cols);
            assert(first.type() == second.type());

            // let CV handle the parallelization
            Mat diffs;
            absdiff(first, second, diffs);
            Scalar s = sum(diffs);

            double total = 0.0;
            for (int i = 0; i<first.channels(); i++){
                total += s[i];
            }

            //cout << "dist is: " << dist/image_pixel_count << endl;
            return total/image_pixel_count;
        }

    public:

        // Default constructor
        NearestNeighbour(){}
        
        // Parameter constructor
        NearestNeighbour(vector<Dataset> data): data(data) {

            cout << "data count: " << data.size() << endl;
            cout << "image pixel count: " << data[0].image.total() << " (" << data[0].image.size() << ")" << endl;

            image_pixel_count = data[0].image.total();

            // apply preproessing
            preprocess_data(&data);

            int result = ProcessData();
            if (!result) {cout << "error occured in mapping data, aborting" << endl; exit(1);}

            for (int i = 0; i<mapped_data.size(); i++){

                cout << "label: " << mapped_data[i].first << " - " << mapped_data[i].second.size() << endl;
            }
        }

        // Destructor
        ~NearestNeighbour(){
            cout << "Algorithm deleted" << endl;
        }

        int classify(const Mat& test_image) const {
            /*
                Classifies given image with L1 distance, returns the lowest distance label
                // TODO: Recheck if this still works
            */

            // lowest label and its value, we'll update it after computing every label
            int lowest_label = 11;
            double lowest_value = numeric_limits<double>::max();;
            
            for (int label = 0; label<mapped_data.size(); label++){
                double distance = 0.0;

                // get the label from map and its image indexes
                int data_label = mapped_data[label].first;
                vector<int> samples_vec = mapped_data[label].second;

                // Iterate over the index numbers for that label (class)
                for (int i = 0; i<samples_vec.size(); i++){

                    // get the image
                    Mat image = data[samples_vec[i]].image;

                    // get the L1 distance
                    double result = compute_L1_distance(image, test_image);
                    distance += result;
                }

                // average the distance to the sample size, 
                // so that classes with larger samples don't get higher distance due to their sample count
                distance /= samples_vec.size();

                // update lowests
                if (distance < lowest_value){lowest_value = distance; lowest_label = data_label;}
            }

            return lowest_label;
        }

};

void custom_test_thread(NearestNeighbour* nn, vector<Dataset>* test, int start, int end, int* true_count){
    /*
        Thread function for multithreading
    */

    int local_true = 0;
    for (int i = start; i<end; i++){

        int prediction = nn->classify((*test)[i].image);
        if (prediction == (*test)[i].label){local_true++;}
    }

    *true_count = local_true;
}


// TODO: Apply preprocessing steps from utils.cpp

// to compile and run: g++ ./Nearest_Neighbour.cpp utils.cpp -o Nearest_Neighbour -I/usr/local/include/opencv4 -lopencv_core && ./Nearest_Neighbour

int main(){

    auto start = chrono::high_resolution_clock::now();

    string data_path = "/home/debianokkes/Downloads/cifar-10-batches-bin/data_batch_1.bin";
    vector<Dataset> data = read_cifar_bin(data_path);

    cout << data[0].image.size() << " - " << data[0].image.total() << endl;

    // concat other data 
    for (int i = 2; i<6; i++){
        string data_path_local = "/home/debianokkes/Downloads/cifar-10-batches-bin/data_batch_" + to_string(i) + ".bin";
        vector<Dataset> data_local = read_cifar_bin(data_path_local);

        data.insert(data.end(), data_local.begin(), data_local.end());
    }

    // get the test data
    string test_path = "/home/debianokkes/Downloads/cifar-10-batches-bin/test_batch.bin";
    vector<Dataset> test = read_cifar_bin(test_path);


    // init model
    NearestNeighbour nn = NearestNeighbour(data);
    cout << "test size: " << test.size() << endl;

    int multi_threading = 1;
    int total_true = 0;

    if (multi_threading == 1){ // apply multithreaded way
    
        // adjust thread count
        int thread_count = 14; // 17 seconds thread::hardware_concurrency() | 
        int proportion = test.size() / thread_count;
    
        cout << "Threads: " << thread_count << " Proportion per thread: " << proportion << endl;
    
        // hold threads and their results in vec
        vector<int> true_count_vec(thread_count);
        vector<thread> all_threads;
    
        for (int i = 0; i<thread_count; i++){
    
            int end = (i == (thread_count - 1)) ? test.size() : proportion * (i+1);
            int start = proportion * i;
            all_threads.push_back(thread(custom_test_thread, &nn, &test, start, end, &true_count_vec[i]));
        }
        
    
        for (auto& thread : all_threads){
            thread.join();
        }
    
        for (int i = 0; i<true_count_vec.size(); i++){
            cout << true_count_vec[i] << " ";
        }
        cout << endl;
        total_true = accumulate(true_count_vec.begin(), true_count_vec.end(), 0);
    } else { // sequantial method
        
        for (int i = 0; i<test.size(); i++){
            int prediction = nn.classify(test[i].image);
            if (prediction == test[i].label){total_true++;}
        }
    
    }

    cout << "Accuray: %" << ((double)total_true/(double)test.size())*100 << endl;
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << " seconds\n";

    return 0;
}