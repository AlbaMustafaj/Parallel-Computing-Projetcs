#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

/*The following program behaves ambiguously, it sometimes crashes
because of the for loops even if they are empty, sometimes does not allow
allocating a new array. If I try in a different pc the for loops work but
the matrix is not read. What is also ambiguous is that even if I comment out
all the send and receive part, the same calculations  which work fine in the other
smooth version are left but in this case the program stop.Please look at the code
and explanations as I believe it is a problem with C or some unknown feature that
is causing calculations to stop and sometimes not even going to the send and receive part . */
int main (int argc, char** argv) {

    MPI_Init(&argc, &argv);
    //get the nr of processors
    int processorsNr;
    MPI_Comm_size(MPI_COMM_WORLD, &processorsNr);
    //get the current processor
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int update;
    int nrOfRows;
    printf("Before checking Core:%d\n", processId);

    if (processId == 0) {
        FILE *matrixData;
        FILE *kernel;
        double kernelMatrix[3][3];

        matrixData = fopen("image.txt", "r");
        kernel = fopen("txt", "r");

        //check matrix
        if (matrixData != NULL) {
            printf("Matrix File opened\n");
        } else {
            printf("No such matrix file found");
        }
        //get kernel data into array
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                char c;
                if (fscanf(kernel, " %c", &c) != 1) {
                    printf("No kernel");
                    exit(1);
                } else if (isdigit((unsigned char) c)) {
                    kernelMatrix[i][j] = c - '0';
                    printf("%f", kernelMatrix[i][j]);
                } else
                    kernelMatrix[i][j] = 0;
            }
            printf("\n");
        }
        fclose(kernel);

        printf("\n");

        char *str;
        //get first line to find the dimension  fscanf (matrixData, "%d", &nrOfRows);
        char *c = fgets(str, sizeof(str), matrixData);
        nrOfRows = atoi(c);
        printf("Nr of rows is %d\n", nrOfRows);
        //get matrix data into array
        int matrix[nrOfRows][nrOfRows];
        for (int i = 0; i < nrOfRows; i++) {
            for (int j = 0; j < nrOfRows; j++) {
                char c;
                if (fscanf(matrixData, " %c", &c) != 1) {
                    printf("No matrix");
                    exit(1);
                } else if (isdigit((unsigned char) c)) {
                    matrix[i][j] = c - '0';
                    // printf("%d", matrix[i][j]);
                } else
                    matrix[i][j] = 0;
            }
            // printf("\n");
        }
        fclose(matrixData);

        double sumKernel =0; //find sum of kernel elements
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                sumKernel += kernelMatrix[i][j];
            }
        }
        printf("Sum found %f ", sumKernel);

        //divide each element by the sum
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                kernelMatrix[i][j] = (kernelMatrix[i][j] / sumKernel);
            }
        }

        //find how many rows to send at each processor
        int dimensionToSend = nrOfRows / processorsNr;
        update = dimensionToSend;
        printf("There are %d rows to send to each processor \n", dimensionToSend);

        //send part
        for (int i = 0; i < processorsNr; i++) {
            printf("Sending from master to others\n");
            //send dimensions
            MPI_Send(&update, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&nrOfRows, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            //send kernel
            MPI_Send(&(kernelMatrix[0][0]), 3 * 3, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
            //send part of matrix to each process;
            if ( i == (processorsNr -1)) { //if last prcessor send 1 less
                MPI_Send(&(matrix[update * i - 1][0]), (dimensionToSend + 1) * nrOfRows, MPI_INT, i, 3, MPI_COMM_WORLD);
            }
            else {
                MPI_Send(&(matrix[update * i - 1][0]), (dimensionToSend + 2) * nrOfRows, MPI_INT, i, 3, MPI_COMM_WORLD);
            }

        }

        int arrayForPadding[dimensionToSend + 1][nrOfRows + 2]; //make another array for 0 padding
        //put 0 on the sides and the matrix chunk to be calculated from master process
        for (int i = 0; i < dimensionToSend+1; i++) {
            for (int j = 0; j < nrOfRows+2; j++) {
                if (i>= 1 && i < dimensionToSend && j >=1 && j < nrOfRows+1) {
                    arrayForPadding[i][j] = matrix[i-1][j-1];
                    //  printf("%f", arrayForPadding[i][j]);
                }
                else {
                    arrayForPadding[i][j] = 0;
                }
            }
        }



        //master makes calculations
        double sum = 0;
        for (int i = 1; i < dimensionToSend; i++) {
            for (int j = 1; j < nrOfRows+1; j++) {

                sum = kernelMatrix[1][1] * arrayForPadding[i][j] + kernelMatrix[0][0] * arrayForPadding[i - 1][j - 1] +
                      kernelMatrix[0][1] * arrayForPadding[i - 1][j] +
                      kernelMatrix[0][2] * arrayForPadding[i - 1][j + 1] +
                      kernelMatrix[1][0] * arrayForPadding[i][j - 1] + kernelMatrix[1][2] * arrayForPadding[i][j + 1] +
                      kernelMatrix[2][0] * arrayForPadding[i + 1][j - 1] +
                      kernelMatrix[2][1] * arrayForPadding[i + 1][j] +
                      kernelMatrix[2][2] * arrayForPadding[i + 1][j + 1];
                matrix[i - 1][j - 1] = sum;
                sum = 0;
            }
        }

        printf("Receiving from all processors to master\n"); //master receives from all
        for (int i = 1; i < processorsNr; i++) {
            MPI_Recv(&matrix[dimensionToSend*i][nrOfRows], dimensionToSend*nrOfRows, MPI_DOUBLE, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        }

    }
    else {

        printf("Receiving to all processors from master\n");
        //receive dimensions
        MPI_Recv(&update, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&nrOfRows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int imagePiece[update][nrOfRows]; //new array containing the matrix chunk size

        double kernelMatrix[3][3]; //receive kernell
        MPI_Recv(&(kernelMatrix[0][0]), 3*3, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&(imagePiece[0][0]), update*nrOfRows, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int arrayForPadding[update + 1][nrOfRows + 2]; //copy in another array for padding
        //for last processor
        if (processId == processorsNr-1) {
            for (int i = 0; i < update; i++) {
                for (int j = 0; j < nrOfRows; j++) {
                    arrayForPadding[update - 1][j] = 0;
                    arrayForPadding[i][0] = 0;
                    arrayForPadding[i][nrOfRows - 1] = 0;
                    arrayForPadding[i][j + 1] = imagePiece[i][j];
                }
            }
        }
        else {
            //for other processors
            for (int i = 0; i < update; i++) {
                for (int j = 0; j < nrOfRows; j++) {
                    arrayForPadding[i][0] = 0;
                    arrayForPadding[i][nrOfRows - 1] = 0;
                    arrayForPadding[i][j + 1] = imagePiece[i][j];
                }
            }
        }

        //calculations
        double sum = 0;
        for (int i = 1; i < update; i++) {
            for (int j = 1; j < nrOfRows; j++) {

                sum = kernelMatrix[1][1] * arrayForPadding[i][j] + kernelMatrix[0][0] * arrayForPadding[i - 1][j - 1] +
                      kernelMatrix[0][1] * arrayForPadding[i - 1][j] +
                      kernelMatrix[0][2] * arrayForPadding[i - 1][j + 1] +
                      kernelMatrix[1][0] * arrayForPadding[i][j - 1] + kernelMatrix[1][2] * arrayForPadding[i][j + 1] +
                      kernelMatrix[2][0] * arrayForPadding[i + 1][j - 1] +
                      kernelMatrix[2][1] * arrayForPadding[i + 1][j] +
                      kernelMatrix[2][2] * arrayForPadding[i + 1][j + 1];

                imagePiece[i - 1][j - 1] = rint(sum); //make the sum an int
                sum = 0;
            }
        }

        //send back to master the results

        MPI_Send(&(imagePiece[0][0]),  update* nrOfRows, MPI_INT, 0, 4, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}