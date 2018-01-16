#include <stdio.h>
#include "utils.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    //get the nr of processors
    int processorsNr;
    MPI_Comm_size(MPI_COMM_WORLD, &processorsNr);
    //get the current processor
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    int buff;


    //k reduce function, if process id is 0 then it receives 2 arrays from each process and
    //stores them in the big array with size k*nrOfProcessors by keeping track of the ids as
    // well accordingly. If process id != 0, 2 arrays are send to the master id holding the
    // top k selected simmilar elements
    void kreduce(int * topk, int * myids, int * myvals, int k, int world_size, int my_rank) {
       // printf("Kreduce is being executed\n");
        int arraySize = processorsNr * k;
        int space = k;
        int finalArray[arraySize];
        int finalIds[arraySize];
        if (processId != 0) {
           // printf("The nr of processors for this process is %d \n", processorsNr);
          //  printf("Sending from kreduce to master\n");
            MPI_Send(myids, k, MPI_INT, 0, 7, MPI_COMM_WORLD); //send ids
            MPI_Send(myvals, k, MPI_INT, 0, 8, MPI_COMM_WORLD); //send myvals
        }

        if (processId == 0) {
           // printf("master receiving in kreduce\n");
            for (int i = 0; i <k; i++) {
                finalArray[i] = myvals[i];
                finalIds[i] = myids[i];
            }
            for (int i = 1; i <processorsNr; i++) {
                MPI_Recv(&finalIds[space], k, MPI_INT, i, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  //receive ids
                MPI_Recv(&finalArray[space], k, MPI_INT, i, 8, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive myvals
                space = space+k; //avoid collapsing indexes
            }

            mergeSort(finalArray,finalIds,0,arraySize-1); //sort the big array with all the data received
          /*  for (int i = 0; i < arraySize; i++) { // test if mergesort worked
                printf("%d \n", finalArray[i]);
            }
            printf("-------------------\n"); */
            printf("Top k = %d id:  \n", k);
            for (int i = 0; i < k; i++) {
                topk[i] =  finalIds[i];         //put only k ids in the topk array
                printf(" %d \n",  topk[i]);
            }
        }
    }
    //-----------------------------------IF MASTER----------------------------------------------
    if (processId == 0) {
        double serialStart = MPI_Wtime();
        int nrOfCols = atoi(argv[1]); // size of dictionary
        int topDocs = atoi(argv[2]); //top related documents
        char* documents = argv[3];
        char* query= argv[4];
        printf("There are %d top documents you have to find\n", topDocs);

    FILE* queryFile = fopen("query.txt", "r"); //read query file so we learn the nr of elements in a row
    if (queryFile == NULL) {
        printf("No such query file\n");
    }
    else {
        printf("Successfully opened query\n");
    }

    FILE* setOfDocs = fopen("documents.txt", "r"); //read set of docs
    if (setOfDocs == NULL) {
        printf("No such documents file\n");
    }
    else {
        printf("Successfully opened documents\n");
    }

    int nrOfRows = getNrOfRows(setOfDocs); //get nr of elements in a line = columns for the arrays
    int querryArr [nrOfCols];
    //make the querry array

     printf("There are %d rows in total in the documents (Nr of files) \n", nrOfRows);
    readQuery (queryFile, nrOfCols, querryArr);

    //put the documents in a 2D array and the ID's in another array
    int docsArr [nrOfRows][nrOfCols];
    int idArr [nrOfRows];
    readDictionary(setOfDocs, nrOfRows, nrOfCols, docsArr, idArr);

  //  --------------------------------MPI PART ----------------------------------------------------

        //number of elements to be sent to each process
        //take into account that master may take more if the nr of rows is not divisible by processorsNr
        int extraforMaster = nrOfRows % processorsNr;
        int elementsToEachProcess = nrOfRows / processorsNr;
        buff = elementsToEachProcess + extraforMaster;
        printf("Elements that each core should process is %d\n", elementsToEachProcess);

        //send elements to each process. The master also has the same amount
       // int dataToProcess[elementsToEachProcess];

        double serialEnd = MPI_Wtime();
        double time_taken = serialEnd-serialStart;

        //nr of k cannot be bigger than the total nr of elements in the master process
        if (topDocs > elementsToEachProcess) {
            topDocs = elementsToEachProcess;
        }

        double startTime =MPI_Wtime();
        for (int i = 1; i < processorsNr; i++) {

            MPI_Send(&topDocs, 1, MPI_INT, i, 2, MPI_COMM_WORLD); //send the number of top K documents
            MPI_Send(&nrOfCols, 1, MPI_INT, i, 3, MPI_COMM_WORLD); //send size of querry array
            MPI_Send(&querryArr, nrOfCols, MPI_INT, i, 4, MPI_COMM_WORLD); //send querry array

            MPI_Send(&elementsToEachProcess, 1, MPI_INT, i, 1, MPI_COMM_WORLD); // nr of elements in the chunk of id array
            MPI_Send(&idArr[buff], elementsToEachProcess, MPI_INT, i, 5, MPI_COMM_WORLD); //chunk of id array
            MPI_Send(&(docsArr[buff][0]), elementsToEachProcess * nrOfCols, MPI_INT, i, 6, MPI_COMM_WORLD);
            buff = buff + elementsToEachProcess;
        }

        //-------------Master performing simmilarity process------------------------------
        int mastersize = elementsToEachProcess+extraforMaster;
        int similarityArr[mastersize]; //apply the similarity function to each row of the document
        for (int i = 0; i < mastersize; i++) {
            similarityArr[i] = similarityFnc(nrOfCols, docsArr, i, querryArr);
           //  printf("Result for %d row in master process is %d \n", i, similarityArr[i]);
        }

        //sort elements in master

        mergeSort(similarityArr, idArr, 0, elementsToEachProcess-1);

       // for (int i = 0; i < elementsToEachProcess; i++) {

          //  printf("The elements in descending order are %d \n", similarityArr[i]);
          //  printf("The ids in descending order are %d \n", idArr[i]);

     //   }
        //------------------KREDUCE call for master----------------------------------------------------

        int myids [topDocs];
        int myvals [topDocs];
        for (int i = 0; i < topDocs; i++) {
            myids[i] = idArr[i];
            myvals[i] = similarityArr[i];
        }
        int topk [topDocs];
        kreduce(topk, myids, myvals, topDocs, 0,0);
      //  for (int i = 0; i< topDocs; i++) {
        //    printf("The topk ids are %d \n", topk[i]);
       // }
        double end = MPI_Wtime();
        double time_paralel= end - startTime;
        printf("Sequential Part: %f  \n", time_taken);
        printf("Paralel part: %f  \n", time_paralel);
        printf("Total time: %f  \n", time_paralel+time_taken);

    }
        /////----------------------1....n processes work-------------------------------
    else {
        int querrySize;
        int topDocs;
        //printf("Receiving to %d processors from master\n", processId);
        MPI_Recv(&topDocs, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive the number of top K documents
        MPI_Recv(&querrySize, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive size of querry Array
        //printf("Query size is %d \n", querrySize);
        int querryArr [querrySize];
        MPI_Recv(&querryArr, querrySize, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive query array

        MPI_Recv(&buff, 1, MPI_INT, 0, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE); //receive nr of elements of the chunk of id array
        //printf("nr of elemeents of chunk array(else part) is %d \n", buff);
        int idArr [buff];
        MPI_Recv(&idArr, buff, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive  the chunk of the id array

        int docsChunk[buff][querrySize];
        MPI_Recv(*docsChunk, buff * querrySize, MPI_INT, 0, 6, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE); //receive  the chunk of documents array

        int similarityArr[buff]; //apply the similarity function to each row of the document
        for (int i = 0; i < buff; i++) {
            similarityArr[i] = similarityFnc(querrySize, docsChunk, i, querryArr);
            // printf("Result for %d row is %d \n", i, similarityArr[i]);
        }

            mergeSort(similarityArr, idArr, 0, buff-1);
           // printf("Merge completed in processor %d \n", processId);


      //  for (int i = 0; i < buff; i++) {

          // printf("The elements in descending order are %d \n", similarityArr[i]);
         //   printf("The ids in descending order are %d \n", idArr[i]);

      //  }
        //assign myids and myvals arrays
        //printf("Top docs are %d \n", topDocs );
        int myids [topDocs];
        int myvals [topDocs];
        for (int i = 0; i < topDocs; i++) {
            myids[i] = idArr[i];
            myvals[i] = similarityArr[i];
        }
        kreduce(myids, myids, myvals,topDocs,0,processId);


    }


    MPI_Finalize();
    return 0;
}