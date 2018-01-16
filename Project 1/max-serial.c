#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Finding max!\n");
	clock_t start;
	clock_t end;
	start = clock();
    FILE *theInput;
    theInput = fopen("input.txt", "r");

    if (!theInput) {
        printf("File couldn't be found, enter an existing path!");
        exit(-1);
    }

    //to be assigned the max nr
    int max = 0;

    int integer;
    while(fscanf(theInput, "%d", &integer) > 0) {
        if (integer > max) {
            max = integer;
        }
    }

    printf("%d\n",max);

    //close file
    fclose(theInput);
	end = clock();
	printf("The time elapsed is: %f ", ((double)((end - start)*CLOCKS_PER_SEC)));
    return 0;
}