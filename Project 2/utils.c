//
// Created by pc on 2017-11-07.
//

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

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


void skip1(char **str) { //skips the spaces
    int size = -1;
    do {
        if((*str)[size] == '\n') {
            return;
        }
        size++;
    } while((*str)[size] != ':');
    (*str) += (size +1);
}

int getNrOfRows(FILE* someFile) { //get the number of rows
   // printf("Inside nr of rows function\n");
    int rowNr =0;
    char *str;
    char nn[256];
    str = fgets(nn, sizeof(nn), someFile);
    while (!feof(someFile)) {
                rowNr++;
        str = fgets(nn, sizeof(nn), someFile);
        }
    rewind(someFile);
    return  rowNr;
}

void readDictionary(FILE* setOfDocs, int rows, int cols, int documents [][cols], int id[rows]) {
   // printf("Inside read dictionary\n");
    char *str;
    char nn[256];
    for (int i=0; i <rows; i++) {
        str = fgets(nn, sizeof(nn), setOfDocs);
        id[i] = atoi(str);
       // printf(" %d ", id[i]);
        skip1(&str);
        skip(&str);
        for (int j = 0; j < cols; j++) {
            documents[i][j] = atoi(str);
         //  printf(" %d ", documents[i][j]);
            skip(&str);
        }
      //  printf("\n");
    }
    fclose(setOfDocs);
}

void readQuery (FILE*queryFile, size_t elements, int queryArr []) {
   // printf("Inside read query\n");
    char *str;
    char nn[1000*elements];
    str = fgets(nn, sizeof(nn), queryFile);
    for (int i=0; i <elements; i++) {
        queryArr[i] = atoi(str);
           // printf("%d", queryArr[i]);
            skip(&str);
        }
    fclose(queryFile);
    }

int similarityFnc( int cols,int docsChunk [][cols], int i, int querryArr[]) {
    int result = 0;
    for (int j = 0; j < cols; j++) { //apply function to each row
        result +=  pow(docsChunk[i][j], querryArr[j]);
    }
   // printf("Result is %d \n", result);
    return  result;
}

void merge(int chunkArr[], int idArr [], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
  // temp arrs
   int L[n1], R[n2];
    int idL[n1], idR[n2];
   /* copy data to temp arrs */
    for (i = 0; i < n1; i++) {
        L[i] = chunkArr[l + i];
        idL[i] = idArr[l + i];
    }
    for (j = 0; j < n2; j++) {
        R[j] = chunkArr[m + 1 + j];
        idR[j] = idArr[m+1 +j];
    }

    // Merge back temp arrs
    i = 0; // Init index of first subarr
    j = 0; // Init index of second subarr
    k = l; // Init index of merged
    while (i < n1 && j < n2)
    {
        if (L[i] >= R[j])
      {
            chunkArr[k] = L[i];
            idArr[k] = idL[i];
           i++;
        }
        else
       {
           chunkArr[k] = R[j];
           idArr[k] = idR[j];
            j++;
       }
       k++;
    }
    // Copy the remaining elements (if any)
    while (i < n1) {
        chunkArr[k] = L[i];
        idArr[k] = idL[i];
        i++;
      k++;
    }
    while (j < n2)
    {
        chunkArr[k] = R[j];
        idArr[k] = idR[j];
        j++;
        k++;
    }
}


void mergeSort(int chunkArr[], int idArr[], int l, int r)
{
    if (l < r) {
        // find middle
        int m = l + (r - l) / 2;
        // Sort halves
        mergeSort(chunkArr, idArr, l, m);
        mergeSort(chunkArr, idArr, m + 1, r);
        merge(chunkArr, idArr, l, m, r);
    }
}