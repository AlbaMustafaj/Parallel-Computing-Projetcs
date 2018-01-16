#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdbool.h>

int findMax(int theId, int elementsToProcess, int data []) {
    int max = data[0];

    for (int i = 0; i < elementsToProcess; i++) {
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}


int main (int argc, char** argv) {

    MPI_Init(&argc, &argv);
    //get the nr of processors
    int processorsNr;
    MPI_Comm_size(MPI_COMM_WORLD, &processorsNr);
    //get the current processor
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);


    printf("Before checking Core:%d\n", processId);
    
    FILE *inputs;
    int totalElements = 0;
    int theMax = 0;
    int theMaxx =0;

    double theTime =  MPI_Wtime(); //before work
    if (processId == 0) {

        inputs = fopen("input.txt", "r");
        if (inputs != NULL) {

            char maxAllowed[255];
            char *charArr;

            while (true) {
                charArr = fgets(maxAllowed, sizeof(maxAllowed), inputs);
                if (charArr == NULL) {
                    break;
                }
                totalElements++;
            }
            rewind(inputs);
            printf("File is read   !\n");
            printf("Number of elements is %d\n", totalElements);
        } else {
            printf("Not found");
            return 0;
        }

        //move the contents of the file to an array
        int data[totalElements];
        char maxAllowed[255];
        char *charArr;

        for (int i = 0; i < totalElements; i++) {
            charArr = fgets(maxAllowed, sizeof(maxAllowed), inputs);
            data[i] = atoi(charArr);
        }
        //dont need file anymore
        fclose(inputs);
        //send number of elements
        MPI_Bcast (&totalElements, 1, MPI_INT,0, MPI_COMM_WORLD);

        //send array to all processes
        MPI_Bcast (data, totalElements, MPI_INT,0, MPI_COMM_WORLD);
        theMax = findMax(processId,  totalElements, data);

    }
    else {
        int length;
        //receive length
        MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int* dataToProcess = (int*)malloc(sizeof(int)*length);
        //rceive array
        MPI_Bcast (dataToProcess, length, MPI_INT,0, MPI_COMM_WORLD);
        //find max
        printf("Finding max in all to all reduce\n");
        theMax = findMax(processId,  length, dataToProcess);

    }
    MPI_Allreduce(&theMax, &theMaxx, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    printf("The max is %d\n", theMaxx);
    theTime = MPI_Wtime() - theTime;  /* after work*/
    printf(" Time elapsed:  %lf\n", theTime);
        MPI_Finalize();
        return 0;
    }

