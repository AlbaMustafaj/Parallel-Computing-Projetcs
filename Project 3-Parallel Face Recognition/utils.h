
// Created by pc on 2017-12-01.

#ifndef PROJECT3_UTILS_H
#define PROJECT3_UTILS_H
//
#include <cstdio>
#include <iostream>
#include <string>


/*
*	Allocates an rxc integer matrix
*/
int ** alloc_2d_matrix(int r, int c);

/*
*	Deallocates an rxc integer matrix
*/
void dealloc_2d_matrix(int ** a, int r, int c);

/*@params:
*		file_name: name of the file
*		h: number of rows in the file
*		w: number of columns in the file
*		reads a matrix file
*		note that this function can not read pgm files, only use with given dataset
**/
int ** read_pgm_file( std:: string file_name, int h, int w);

void create_histogram(int * hist, int ** img, int num_rows, int num_cols);
double distance(int * a, int *b, int size);
int find_closest(int ***training_set, int num_persons, int num_training, int size, int *
test_image);

#endif //PROJECT3_UTILS_H
