#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
int **alloc_2d_matrix(int r, int c)
{
    int ** a;
    int i;
   // a = (int **)malloc(sizeof(int *) * r);
   a = new int*[r];
    if (a == nullptr) {
        cerr << "memory allocation failure" << endl;

    }
    for (i = 0; i < r; ++i) {
        a[i] = new int [c];
       if (a[i] == nullptr) {
           cerr << "memory allocation failure" << endl;
        }
    }
    return a;
}

void dealloc_2d_matrix(int **a, int r, int c)
{
    	int i;
   	for (i = 0; i < r; ++i) {
        delete a[i];
    }
    delete []a;
}

int ** read_pgm_file(string file_name, int h, int w)
{
    ifstream File;
    File.open(file_name);
    //cout << file_name << endl;
    	if (!File.is_open())
	{
	    cerr << "ERROR: file open failed" << endl;
	    return(nullptr);
	}
    int** data=alloc_2d_matrix(h,w);
    int tmp;
    for (int i = 0; i < h;i++) {
        for (int j = 0; j < w; j++) {
            File >> tmp;
            data[i][j] = tmp;
        //   cout <<data[i][j] ;

        }

    }

    File.close();
    return data;
}