#include <iostream>
#include <iostream>
#include "utils.h"
#include <string>
#include <sstream>
#include <cmath>
using namespace std;
void create_histogram(int *hist, int **img, int num_rows, int num_cols){
    int  smallMatrix[3][3];
    int i = 1;
    int decimal = 0;
    while ( i <= num_rows) {
        int j = 1;
        while ( j <= num_cols) {
            if (img[i][j] <= img[i - 1][j - 1]) {
                smallMatrix[0][0] = 0;
                //cout << img[i][j] << " " << endl;
            }
            else{
                smallMatrix[0][0] = 1;
                // cout << img[i][j] << " " << endl;
            }
            if (img[i][j] <= img[i - 1][j]) {
                smallMatrix[0][1] = 0;
            }
            else  {
                smallMatrix[0][1] = 1;

            }
            if (img[i][j] <= img[i - 1][j + 1]) {
                smallMatrix[0][2] = 0;
            }
            else {
                smallMatrix[0][2] = 1;
            }
            if (img[i][j] <= img[i][j - 1]) {
                smallMatrix[1][0] = 0;
            }
            else {
                smallMatrix[1][0] = 1;
            }
            if (img[i][j] <= img[i][j + 1]) {
                smallMatrix[1][2] = 0;
            }
            else  {
                smallMatrix[1][2] = 1;
            }
            if (img[i][j] <= img[i + 1][j - 1]) {
                smallMatrix[2][0] = 0;
            }
            else {
                smallMatrix[2][0] = 1;
            }
            if (img[i][j] <= img[i + 1][j]) {
                smallMatrix[2][1] = 0;
            }
            else {
                smallMatrix[2][1] = 1;
            }
            if (img[i][j] <= img[i + 1][j + 1]) {
                smallMatrix[2][2] = 0;
            }
            else {
                smallMatrix[2][2] = 1;
            }
            decimal = smallMatrix[0][0] * int(pow(2, 7)) + smallMatrix[0][1] * int(pow(2, 6)) + smallMatrix[0][2] * int(pow(2, 5)) +
                      smallMatrix[1][2] * int(pow(2, 4)) +
                      smallMatrix[2][2] * int(pow(2, 3)) + smallMatrix[2][1] * int(pow(2, 2)) + smallMatrix[2][0] * int(pow(2, 1)) +
                      smallMatrix[1][0] * 1;

            hist[decimal]++;
            // cout <<  hist[decimal] << " " ;
            j++;
        }
        i++;
    }

   // cout<< endl;
}

double distance(int * a, int *b, int size) {
   // printf("inside distance function\n");
    double distance = 0;
  for (int i = 0; i < size; i ++) {
      if (a[i] + b[i] == 0) {
          distance += 0;
      }
      else {
          distance += 0.5 * pow ((a[i]- b[i]), 2) / (a[i] + b[i]);
      }
  }
   // printf("nbefore returning from distance function\n");
    return  distance;
}

int find_closest(int ***training_set, int num_persons, int num_training, int size, int * test_image) {
    double ** dist = new double * [num_persons]; //make an array which will store the comparison values
    for (int i = 0; i < num_persons; i++) {
        dist[i] = new double [num_training];
    }
    for (int i = 0;  i < num_persons; i++) { //populate dhe distance array
        for (int j =0; j < num_training; j++) {
            dist[i][j] = distance(training_set[i][j], test_image, size);
        }
    }

    double closestValue = dist[0][0];
    int  closest = 0 ;

    for (int i = 0;  i < num_persons; i++) {
        for (int j =0; j < num_training; j++) {
            if (dist[i][j] < closestValue){
                closestValue = dist[i][j];
                closest = i;
        }
        }
    }
    for (int i = 0; i < num_persons; ++i) {
        delete dist[i];
    }
    delete []dist;
   // printf("before returning from find closest function\n");
    return closest + 1;
}
int main(int argc, char* argv[] ) {
    char *b = argv[1];
    int k = stoi(b); //take k from argv
	
    int nrOfIds = 18;
    int nrOfPhotosPerId = 20;
    int num_rows = 200;
    int num_cols = 180;
    int histogramSize = 256;
    int start_s=clock();


    int *** training_set = new int **[nrOfIds]; //nr of people, nr of images per person, histogram size
    for (int i = 0; i < nrOfIds; i++) {
        training_set[i] = alloc_2d_matrix(nrOfPhotosPerId,histogramSize);
    }

    for (int i = 0; i < nrOfIds; i++) { //initialize  training set to 0
        for (int j = 0; j < nrOfPhotosPerId; j++) {
            for (int e = 0; e < histogramSize; e++) {
                training_set[i][j][e] = 0;
            }
        }
    }

        //get file name

        string filename;
        for (int w = 1; w <= 18; w++) {
            for (int q = 1; q <= 20; q++) { //get all the file's names

                filename = to_string(w) + "." + to_string(q) + ".txt";


                int **image = read_pgm_file(filename, num_rows, num_cols);
                int **img = alloc_2d_matrix((num_rows + 2), (num_cols + 2)); //enhanced image matrix with 0 in the corners

                for (int i = 0; i < (num_rows + 2); i++) { //initialize enhanced img matrix  0
                    for (int j = 0; j < (num_cols + 2); j++) {
                        img[i][j] = 0;
                    }
                }
                for (int i = 1; i <= num_rows ; i++) { //copy data from the image to enhanced img matrix
                    for (int j = 1; j <= num_cols; j++) {
                        img[i][j] = image[i - 1][j - 1];
                    }
                }

                create_histogram(training_set[w - 1][q - 1], img, num_rows, num_cols);
                //deallocate images
                dealloc_2d_matrix(image, num_rows, num_cols);
                dealloc_2d_matrix(img, (num_rows + 2), (num_cols + 2));
            }
        }

       int tests = 0; //nr of tests in total
       int correctIds = 0; //nr of ids found correctly by comparing
       int testResultId;
         for (int i = 0; i < nrOfIds; i++) {
             for (int j = k; j < nrOfPhotosPerId; j++) {
                testResultId = find_closest(training_set,nrOfIds,k,histogramSize,training_set[i][j]);
                 tests++;
                 printf("%d.%d.txt %d " " %d\n",i+1,j+1,testResultId,i+1);
                // cout << i+i <<"." << j << ".txt" << " " << testResultId << "  " << i+1 << endl;
                 if (testResultId == i+1) {
                     correctIds++;
                 }
             }
         }
    cout << "Accuracy: " << correctIds << " correct answers for " << tests << " tests" << endl;
        for (int i = 0; i < nrOfIds; i++) {
            dealloc_2d_matrix(training_set[i], nrOfPhotosPerId, 256);
        }
    double stop_s=clock();
    cout << "Sequential time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << " ms"<< endl;
        return 0;
    }