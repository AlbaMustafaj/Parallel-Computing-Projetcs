//
// Created by pc on 2017-11-07.
//

#ifndef READFILES_UTILS_H
#define READFILES_UTILS_H

#include <stdio.h>
void skip(char **str);
void skip1(char **str);
int getNrOfRows(FILE* someFile);
void readDictionary(FILE* setOfDocs,  int rows, int cols, int documents [][cols], int id [rows]);
void readQuery (FILE*queryFile, size_t elements, int queryArr []);
int similarityFnc( int cols,int docsChunk [][cols], int i, int querryArr[]);
void mergeSort(int chunkArr[], int idArr[], int l, int r);
void merge(int chunkArr[], int idArr [], int l, int m, int r);
#endif //READFILES_UTILS_H
