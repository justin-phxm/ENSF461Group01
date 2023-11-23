#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
FILE *output = NULL;

// Parse a vector of integers from a file, one per line.
// Return the number of integers parsed.
int parse_ints(FILE *file, int **ints)
{
    int value;
    int size = 0; // Number of ints stored.

    // Allocate memory for the array dynamically.
    *ints = NULL;

    // Read integers one by one.
    while (fscanf(file, "%d", &value) == 1)
    {
        // Reallocate the array to make room for the new integer.
        int *temp = realloc(*ints, (size + 1) * sizeof(int));
        if (!temp)
        {
            perror("Realloc failed");
            free(*ints); // Free any memory that was allocated.
            return -1;
        }
        *ints = temp;
        (*ints)[size++] = value; // Store the integer.
    }

    return size; // Return the number of elements read.
}

// Write a vector of integers to a file, one per line.
void write_ints(FILE *file, const int *ints, int size)
{
    if (file == NULL || ints == NULL)
    {
        fprintf(stderr, "Invalid file pointer or array reference.\n");
        return;
    }

    for (int i = 0; i < size; ++i)
    {
        fprintf(file, "%d\n", ints[i]);
    }
}

// Return the result of a sequential prefix scan of the given vector of integers.
int *SEQ(int *ints, int size)
{
    int *prefixScan = (int *)malloc(size * sizeof(int));
    prefixScan[0] = ints[0];
    for (int i = 1; i < size; i++)
    {
        prefixScan[i] = ints[i] + prefixScan[i - 1];
    }
    return prefixScan;
}

// Return the result of Hillis/Steele, but with each pass executed sequentially
int *HSS(int *ints, int size)
{
    // Initialize prefixScan and other arrays
    int **intermediateArrays = (int **)malloc(log2(size) * sizeof(int *));

    // Initialize step 0
    intermediateArrays[0] = (int *)malloc(size * sizeof(int));
    intermediateArrays[0][0] = ints[0];
    for (int i = 1; i < size; i++)
    {
        intermediateArrays[0][i] = ints[i] + ints[i - 1];
    }

    // Initialize other steps
    for (int i = 1; i < log2(size); i++)
    {
        intermediateArrays[i] = (int *)malloc(size * sizeof(int));

        // Copy the previous step
        for (int j = 0; j < pow(2, i); j++)
        {
            intermediateArrays[i][j] = intermediateArrays[i - 1][j];
        }

        // Calculate the current step
        for (int j = pow(2, i); j < size; j++)
        {
            intermediateArrays[i][j] = intermediateArrays[i - 1][j] + intermediateArrays[i - 1][j - (int)pow(2, i)];
        }
    }

    // Return the prefixScan array
    // return prefixScan;
    return intermediateArrays[(int)log2(size) - 1];
}
// Return the result of Hillis/Steele, parallelized using pthread
typedef struct
{
    int *array;
    int size;
    int stride;
    int start;
    pthread_barrier_t *barrier;
} ThreadData;

void *prefixSum(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int *array = data->array;
    int size = data->size;
    int stride = data->stride;
    int start = data->start;
    pthread_barrier_t *barrier = data->barrier;

    int *intermediateArray = (int *)malloc(size * sizeof(int));
    for (int j = 0; j < size; j++)
    {
        intermediateArray[j] = array[j];
    }
    for (int i = 0; i < log2(size); i++)
    {
        for (int j = start; j < size; j += stride)
        {
            if (j - (int)pow(2, i) >= 0)
            {
                intermediateArray[j] = array[j] + array[j - (int)pow(2, i)];
            }
        }
        pthread_barrier_wait(barrier);
        for (int j = start; j < size; j += stride)
        {
            array[j] = intermediateArray[j];
        }
        pthread_barrier_wait(barrier);
    }
    free(intermediateArray);
    pthread_exit(NULL);
}

int *HSP(int *ints, int size, int numthreads)
{
    int *outputArray = (int *)malloc(size * sizeof(int));
    // copy ints into outputArray
    for (int i = 0; i < size; i++)
    {
        outputArray[i] = ints[i];
    }
    pthread_t threads[numthreads];
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, numthreads);
    ThreadData threadData[numthreads];
    // Initialize Threads
    for (int i = 0; i < numthreads; i++)
    {
        threadData[i].start = i;
        threadData[i].stride = numthreads;
        threadData[i].array = outputArray;
        threadData[i].size = size;
        threadData[i].barrier = &barrier;

        pthread_create(&threads[i], NULL, prefixSum, (void *)&threadData[i]);
    }

    // Join Threads
    for (int i = 0; i < numthreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return outputArray;
}
int main(int argc, char **argv)
{

    if (argc != 5)
    {
        printf("Usage: %s <mode> <#threads> <input file> <output file>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    int num_threads = atoi(argv[2]);
    FILE *input = fopen(argv[3], "r");
    output = fopen(argv[4], "w");

    int *ints;
    int size;
    size = parse_ints(input, &ints);
    // ints[] looks like [1, 2, 3, 4]

    int *result;
    if (strcmp(mode, "SEQ") == 0)
    {
        result = SEQ(ints, size);
    }
    else if (strcmp(mode, "HSS") == 0)
    {
        result = HSS(ints, size);
    }
    else if (strcmp(mode, "HSP") == 0)
    {
        result = HSP(ints, size, num_threads);
    }
    else
    {
        printf("Unknown mode: %s\n", mode);
        return 1;
    }

    write_ints(output, result, size);
    fclose(input);
    fclose(output);
    return 0;
}
