#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

void skip(char **str) { //skips the spaces
    int size = -1;
    do {
        if((*str)[size] == '\n') {
            return;
        }
        size++;
    } while((*str)[size] != 32);
    (*str) += (size +1);
}

int main(int argc, char *argv[]) {
    clock_t start;
    clock_t end;
    FILE *matrixData;
    FILE *kernel;

    double kernelMatrix[3][3];
    //start = clock();
    matrixData = fopen("image.txt", "r");
    kernel = fopen("kernel.txt", "r");
    //check matrix
    if (matrixData != NULL) {
        //printf("Matrix File opened\n");
    }
    else {
        printf("No such matrix file found");
    }

    //get data into kernel array
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            char c;
            if (fscanf(kernel, " %c", &c) != 1) {
                printf("No kernel");
                exit(1);
            }
            else if (isdigit((unsigned char)c)) {
                kernelMatrix[i][j] = c - '0';
              //  printf("%f", kernelMatrix[i][j]);
            }
            else
                kernelMatrix[i][j] = 0;
        }
       // printf("\n");
    }
    fclose(kernel);

    char *str;
    int nrOfRows;

    //get first line to find the dimension  fscanf (matrixData, "%d", &nrOfRows);
    char* c = fgets(str, sizeof(str),matrixData);
     nrOfRows =atoi(c);
    // printf("Nr of rows is %d\n", nrOfRows);
    char nn[1000*nrOfRows];

    int matrix[nrOfRows][nrOfRows];
    //get data for matrix array
    for (int i=0; i <nrOfRows; i++) {
        str = fgets(nn, sizeof(nn), matrixData);
        for (int j=0; j <nrOfRows; j++) {
            matrix[i][j] = atoi(str);
           //printf(" %d ", matrix[i][j]);
            skip(&str);
        }
       // printf("\n");
    }
    fclose(matrixData);
    printf("\n");

    //find sum of kernel matrix
    double sumKernel = 0.0000000;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            sumKernel+=kernelMatrix[i][j];
        }
    }
    //divide each element by sum
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            kernelMatrix[i][j] = (kernelMatrix[i][j] / sumKernel);
           // printf("%f ", kernelMatrix[i][j]);
        }

        // printf("\n");
    }


    //if i declare a variable for the size it does not work
    //gives segmentation fault. I tried many ways there
    //was no way to change the stack or put something like
    // float paddingArray[nrOfRows][nrOfRows]so I just hardcoded it
    //418 was the biggest dimmension it can accept
    float paddingArray[302][302];
    for (int i = 0; i< 302; i++){
       for (int j =0; j< 302; j++) {
           if (i>= 1 && i < 301 && j >=1 && j < 301) {
               paddingArray[i][j] = matrix[i-1][j-1];
              // printf("%f", paddingArray[i][j]);

           }
           else {
               paddingArray[i][j] = 0;
           }
       }
       // printf("\n");
    }
  //  printf("Padding finished");




     double sum;
    //make calculations;
  //printf("The final matrix is being generated...");
    for (int i = 1; i < nrOfRows+1; i++) {
        for (int j =1; j < nrOfRows+1; j++) {
            sum =  kernelMatrix[1][1]*paddingArray[i][j] + kernelMatrix[0][0]*paddingArray[i-1][j-1]+
                kernelMatrix[0][1]*paddingArray[i-1][j] + kernelMatrix[0][2]*paddingArray[i-1][j+1]+kernelMatrix[1][0]*paddingArray[i][j-1] + kernelMatrix[1][2]*paddingArray[i][j+1]+
                kernelMatrix[2][0]*paddingArray[i+1][j-1] + kernelMatrix[2][1]*paddingArray[i+1][j] + kernelMatrix[2][2]* paddingArray[i+1][j+1];

            matrix[i-1][j-1] = rint(sum);
            printf(" %d  ", matrix[i-1][j-1]);
            sum =0;
        }
        printf("\n");
    }

//    end=clock();
//    printf("The time elapsed is: %f ", ((double)(end-start)*CLOCKS_PER_SEC));
    return 0;
}