#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Parse a vector of integers from a file, one per line.
// Return the number of integers parsed.
int parse_ints(FILE *file, int **ints) {

}


// Write a vector of integers to a file, one per line.
void write_ints(FILE *file, int *ints, int size) {

}


// Return the result of a sequential prefix scan of the given vector of integers.
int* SEQ(int *ints, int size) {

}


// Return the result of Hillis/Steele, but with each pass executed sequentially
int* HSS(int *ints, int size) {

}


// Return the result of Hillis/Steele, parallelized using pthread
int* HSP(int *ints, int size, int numthreads) {

}


int main(int argc, char** argv) {

    if ( argc != 5 ) {
        printf("Usage: %s <mode> <#threads> <input file> <output file>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    int num_threads = atoi(argv[2]);
    FILE *input = fopen(argv[3], "r");
    FILE *output = fopen(argv[4], "w");

    int *ints;
    int size;
    size = parse_ints(input, &ints);

    int *result;
    if (strcmp(mode, "SEQ") == 0) {
        result = SEQ(ints, size);
    } else if (strcmp(mode, "HSS") == 0) {
        result = HSS(ints, size);
    } else if (strcmp(mode, "HSP") == 0) {
        result = HSP(ints, size, num_threads);
    } else {
        printf("Unknown mode: %s\n", mode);
        return 1;
    }

    write_ints(output, result, size);
    fclose(input);
    fclose(output);
    return 0;
}
