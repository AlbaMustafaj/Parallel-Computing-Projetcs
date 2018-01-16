#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdbool.h>

int findMax(int theId, int elementsToProcess, int data[]) {
	int max = data[0];

	for (int i = 0; i < elementsToProcess; i++) {
		if (data[i] > max) {
			max = data[i];
		}
	}
	return max;
}


int main(int argc, char** argv) {


	MPI_Init(&argc, &argv);
	double time = MPI_Wtime();
	//get the nr of processors
	int processorsNr;
	MPI_Comm_size(MPI_COMM_WORLD, &processorsNr);
	//get the current processor
	int processId;
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);


	printf("Before checking Core: %d\n", processId);

	int buff;
	//if master:
	if (processId == 0) {
		int max = 0; //hold the max element
		FILE *inputs;
		int totalElements = 0;
		int elementsToEachProcess = 0;
		printf("Core %d starting calculations\n", processId);
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

		}
		else {
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
		//number of elements to be sent to each process
		elementsToEachProcess = totalElements / processorsNr;
		printf("Elements that each core should process is %d\n", elementsToEachProcess);

		//send elements to each process. The master also has the same amount
		int dataToProcess[elementsToEachProcess];
		buff = elementsToEachProcess;


		for (int i = 1; i < processorsNr; i++) {
			printf("Sending from master to others\n");
			MPI_Send(&buff, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

			//send to other processes with different tagg
			MPI_Send(&data[buff], elementsToEachProcess, MPI_INT, i, 2, MPI_COMM_WORLD);
			buff = buff + elementsToEachProcess;
		}

		//master finds the max number of the data he had
		printf("Finding max of master data\n");
		max = findMax(processId, elementsToEachProcess, data);
		int theMax;

		//responses from the other processes:
		printf("Receiving from all processors to master\n");
		for (int i = 1; i < processorsNr; i++) {
			MPI_Recv(&theMax, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (theMax > max) {
				max = theMax;
			}
		}

		printf("Maximum is %d", max);
		time = MPI_Wtime() - time;
		printf("Time elapsed is %f ", time);

	}
	else {
		printf("Receiving to all processors from master\n");
		MPI_Recv(&buff, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int* dataToProcess = (int*)malloc(sizeof(int)*buff);
		MPI_Recv(dataToProcess, buff, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int theMax = findMax(processId, buff, dataToProcess);

		//send results to master

		MPI_Send(&theMax, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		free(dataToProcess);
	}
	
	MPI_Finalize();
	return 0;
}