#include <stdio.h>
#include <cuda.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



__global__ void compute ( int* d_arr1 , int* d_arr2, double* dotPRoduct , double* lenghtOfVector1 , double* lenghtOfVector2 , int dataForEach , int blockSize) {

   int index =  blockSize * blockIdx.x + threadIdx.x; 
   //initialize the corresponding vectors' indexes with 0
   dotPRoduct[index] = 0;
   lenghtOfVector1[index] = 0;
   lenghtOfVector2[index] = 0;
  // int dotprod = 0;
   //int length1 = 0;
   //int length2 = 0;
   int i;
   for ( i = 0; i < dataForEach; i++) {
        dotPRoduct[index] += (d_arr1[dataForEach * index + i] * d_arr2[dataForEach * index + i]);
        lenghtOfVector1[index] += (d_arr1[dataForEach * index + i] * d_arr1[dataForEach * index + i]);
        lenghtOfVector2[index] += (d_arr2[dataForEach * index + i] * d_arr2[dataForEach * index + i]);	

   }
 
}


__host__ int* read(int length, FILE* f){
	int i;
	int* arr = (int*) malloc(length * sizeof(int));
    char ch[150];
	for (i = 0; i < length; i++){
    char* element = fgets(ch, sizeof(ch), f);
	        arr[i] = atoi(element);                 
    }
	return arr;
}

int main(int argc, char **argv)
{
    //get array size
	char* arrSize = argv[1];
	char* blockS = argv[2];
	int size = atoi(arrSize);
	int blockSize = atoi (blockS);
	int blockNumber = size / blockSize;
	
	printf ( "Info\n");
    printf( "——————\n");
    printf("Number of elements: %d \n", size);
    printf("Number of threads per block: %d\n", blockSize);
    printf ("Number of blocks will be created: %d\n\n", blockNumber);
	
    //main arrays
    int* arr1;
    int* arr2;
	double* h_dotProduct;
	double* h_lenghtOfVector1;
	double* h_lenghtOfVector2;
		
	
	// helper arrays for computation
	int* d_arr1;
	int* d_arr2;
	double* dotPRoduct;
	double* lenghtOfVector1;
	double* lenghtOfVector2;
	
	
	//cosine of angle
	double cos;

    //helping cosine formula
    long double dotProd = 0;
    long double sq1 = 0;
    long double sq2 = 0;
    double angle;
	//end

    arr1= (int *)malloc(sizeof(int)*size);
    arr2 = (int *)malloc(sizeof(int)*size);
	h_dotProduct = (double *) malloc((blockSize *blockNumber)*sizeof(double));
	h_lenghtOfVector1 = (double *) malloc((blockSize *blockNumber)*sizeof(double));
	h_lenghtOfVector2 = (double *) malloc((blockSize *blockNumber)*sizeof(double));

    //generate arrays
	clock_t arrGenerationStart, arrGenerationEnd;
	arrGenerationStart = clock();
	if (argc == 3) {
	int iter;
    srand(time(NULL));
    for (iter = 0; iter < size; iter++) {
      arr1[iter] = rand()%10000;
      arr2[iter] = rand()%10000;
      //printf( " %d %d \n",iter, arr1[iter]);
   }
    arrGenerationEnd = clock();
  }
   else if(argc == 4){	 //there is a file
		FILE* f = fopen(argv[3],"r");
		char ch[150];
		char* s = fgets(ch, sizeof(ch), f);
		size = atoi(s); //first number is size (already given in first parameter)
		
		arrGenerationStart = clock();
		arr1 = read(size,f);
		arr2 = read(size,f);
		arrGenerationEnd = clock();
	}
	else{
		printf("Something is wrong with your parameters");
		exit(EXIT_SUCCESS);
	}
   
   
   clock_t cpuFuncStart, cpuFuncEnd;
	cpuFuncStart = clock();
	long double dott = 0;
    long double sqr1 = 0;
    long double sqr2 = 0;
    double cosine;
    double anGle;
	int c;
    for (c =0; c < size; c++) {
        dott += (arr1[c] * arr2[c]);
        sqr1 += arr1[c] * arr1[c];
        sqr2 += arr2[c] * arr2[c];

    }
    // printf("dot product is %Lf \n" ,  dott);

    sqr1 = sqrtl(sqr1);
    //printf("sq1 is %Lf \n" , sqr1);
    sqr2 = sqrtl(sqr2);
   // printf("sq2 is %Lf \n" , sqr2);

    dott = dott / (sqr1 * sqr2);
    cosine = (double) dott;
   // printf("cos is %f \n" , cosine);

    anGle = (double) acos(cosine);
    anGle = 180 * anGle / 3.14;

    //printf("angle is %f\n" , anGle);

	cpuFuncEnd = clock();
   
   //allocate memory in device 
   cudaMalloc( &d_arr1, size*sizeof(int));
   cudaMalloc( &d_arr2, size*sizeof(int));
   cudaMalloc( &dotPRoduct,((blockSize *blockNumber)*sizeof(double)));
   cudaMalloc( &lenghtOfVector1, ((blockSize *blockNumber)*sizeof(double)));
   cudaMalloc( &lenghtOfVector2, ((blockSize *blockNumber)*sizeof(double)));
   
   //copy arrays
   clock_t hostToDeviceTrasfeerStart, hostToDeviceTrasfeerEnd;
   hostToDeviceTrasfeerStart = clock();
   cudaMemcpy(d_arr1, arr1, size*sizeof(int), cudaMemcpyHostToDevice);
   cudaMemcpy(d_arr2, arr2, size*sizeof(int), cudaMemcpyHostToDevice);
   hostToDeviceTrasfeerEnd = clock();

   
   //calculate data for each thread
   int dataForEach =  size / (blockNumber * blockSize);
   
   //call  global function
   clock_t kernelStart, kernelEnd;
     kernelStart = clock();
   compute<<<blockNumber,blockSize>>> (d_arr1, d_arr2, dotPRoduct,lenghtOfVector1,lenghtOfVector2, dataForEach, blockSize);
   cudaThreadSynchronize();
   kernelEnd = clock();
   
   clock_t deviceToHostStart, deviceToHostEnd;
   deviceToHostStart = clock();
   cudaMemcpy(h_dotProduct,dotPRoduct, (blockSize *blockNumber)*sizeof(double), cudaMemcpyDeviceToHost);
   deviceToHostEnd = clock();
   cudaMemcpy(h_lenghtOfVector1,lenghtOfVector1, (blockSize *blockNumber)*sizeof(double), cudaMemcpyDeviceToHost);
   cudaMemcpy(h_lenghtOfVector2,lenghtOfVector2, (blockSize *blockNumber)*sizeof(double), cudaMemcpyDeviceToHost);
      
  // collect data   
   int a;
   for( a =0; a < (blockSize *blockNumber); a++) {
 //      // printf( " %f " ,h_lenghtOfVector1[a]);
		
       dotProd += h_dotProduct[a];
		sq1 += h_lenghtOfVector1[a];
		sq2 += h_lenghtOfVector2[a]; 
  }
   
   //if size not divisible by (blockSize * blockNumber)
   int delivered = dataForEach * blockSize * blockNumber;
   int remaining = size - delivered;
   if (remaining > 0) {
     for (int i = delivered; i < size; i++) {
	    dotProd += arr1[i] * arr2[i];
		sq1 +=  arr1[i] * arr1[i];
		sq2 += arr2[i] * arr2[i];
	 }
   }
    sq1 = sqrtl(sq1);
   // printf("sq1 is %Lf \n" , sq1);
    sq2 = sqrtl(sq2);
   // printf("sq2 is %Lf \n" , sq2);

    dotProd = dotProd / (sq1 * sq2);
    cos = (double) dotProd;
    //printf("cos is %f \n" , cos);

    angle = acos(cos);
    angle = 180 * angle / 3.14;

  //  printf("angle is %f \n" , angle);
	
	printf("Time\n");
    printf("——————\n");
    printf("Time for the array generation : %f ms\n", ((double) (arrGenerationEnd - arrGenerationStart)) /CLOCKS_PER_SEC * 1000);
    printf("Time for the CPU function : %f ms\n", ((double) (cpuFuncEnd - cpuFuncStart)) /CLOCKS_PER_SEC * 1000);
    printf("Time for the Host to Device transfer : %f ms\n", ((double) (hostToDeviceTrasfeerEnd - hostToDeviceTrasfeerStart)) /CLOCKS_PER_SEC * 1000);
    printf("Time for the kernel execution : %f ms\n", ((double) (kernelEnd - kernelStart)) /CLOCKS_PER_SEC * 1000);
    printf("Time for the Device to Host transfer : %f ms\n" , ((double) (deviceToHostEnd - deviceToHostStart)) /CLOCKS_PER_SEC *1000);
    double gpuTime = (hostToDeviceTrasfeerEnd - hostToDeviceTrasfeerStart) + (kernelEnd - kernelStart) + (deviceToHostEnd - deviceToHostStart);
	printf("Total execution time for GPU : %f ms\n\n ", ((double) gpuTime / CLOCKS_PER_SEC) *1000);
	printf("Results\n");
	printf("——————\n");
	printf("CPU result: %f \n", anGle);
	printf("GPU result: %f \n", angle);
    return 0;
	
}