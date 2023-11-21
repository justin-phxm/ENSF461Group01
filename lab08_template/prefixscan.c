#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

typedef struct
{
    int *data;
    int step;
    int start;
    int end;
} thread_args;

void *up_sweep_thread(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int start = args->start;
    int end = args->end;
    int step = args->step;

    for (int i = start; i < end; i += step)
    {
        args->data[i + step - 1] += args->data[i + (step / 2) - 1];
    }

    pthread_exit(NULL);
}

void *down_sweep_thread(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int start = args->start;
    int end = args->end;
    int step = args->step;

    for (int i = start; i < end; i += step)
    {
        int t = args->data[i + (step / 2) - 1];
        args->data[i + (step / 2) - 1] = args->data[i + step - 1];
        args->data[i + step - 1] += t;
    }

    pthread_exit(NULL);
}

void parallel_scan(int *data, int length, int num_threads)
{
    pthread_t threads[num_threads];
    thread_args args[num_threads];

    // Up-sweep phase
    for (int d = 0; d < log2(length); ++d)
    {
        int step = pow(2, d + 1);
        for (int i = 0; i < num_threads; ++i)
        {
            args[i].data = data;
            args[i].step = step;
            args[i].start = i * step;
            args[i].end = fmin(length, (i + 1) * step);
            if (pthread_create(&threads[i], NULL, up_sweep_thread, &args[i]))
            {
                fprintf(stderr, "Error creating thread\n");
                exit(1);
            }
        }
        for (int i = 0; i < num_threads; ++i)
        {
            pthread_join(threads[i], NULL);
        }
    }

    data[length - 1] = 0; // Set last element to zero for exclusive scan

    // Down-sweep phase
    for (int d = log2(length) - 1; d >= 0; --d)
    {
        int step = pow(2, d + 1);
        for (int i = 0; i < num_threads; ++i)
        {
            args[i].data = data;
            args[i].step = step;
            args[i].start = i * step;
            args[i].end = fmin(length, (i + 1) * step);
            if (pthread_create(&threads[i], NULL, down_sweep_thread, &args[i]))
            {
                fprintf(stderr, "Error creating thread\n");
                exit(1);
            }
        }
        for (int i = 0; i < num_threads; ++i)
        {
            pthread_join(threads[i], NULL);
        }
    }
}

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
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int *array;
    int *sums;
    int size;
    int thread_id;
    int num_threads;
    pthread_barrier_t *barrier;
} ThreadData;

void *prefixSum(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int *arr = data->array;
    int *sums = data->sums;
    int size = data->size;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;
    pthread_barrier_t *barrier = data->barrier;

    int start = thread_id * (size / num_threads);
    int end = (thread_id + 1) * (size / num_threads);
    if (thread_id == num_threads - 1)
    {
        end = size;
    }

    for (int stride = 1; stride < size; stride *= 2)
    {
        int prev_sum = thread_id > 0 ? sums[thread_id - 1] : 0;
        for (int i = start; i < end; i++)
        {
            if (i >= stride)
            {
                arr[i] += arr[i - stride] + prev_sum;
            }
        }

        // Update sums array
        sums[thread_id] = arr[end - 1];
        if (thread_id > 0)
        {
            sums[thread_id] += sums[thread_id - 1];
        }

        pthread_barrier_wait(barrier);
    }

    pthread_exit(NULL);
}

int *HSP(int *ints, int size, int numthreads)
{
    int *prefixScan = (int *)malloc(size * sizeof(int));
    int *sums = (int *)malloc(numthreads * sizeof(int));
    if (prefixScan == NULL || sums == NULL)
    {
        perror("Malloc failed");
        return NULL;
    }

    for (int i = 0; i < size; i++)
    {
        prefixScan[i] = ints[i];
    }

    pthread_t threads[numthreads];
    ThreadData threadData[numthreads];
    pthread_barrier_t barrier;

    pthread_barrier_init(&barrier, NULL, numthreads);
    for (int i = 0; i < numthreads; i++)
    {
        sums[i] = 0;
    }

    for (int i = 0; i < numthreads; i++)
    {
        threadData[i].array = prefixScan;
        threadData[i].sums = sums;
        threadData[i].size = size;
        threadData[i].thread_id = i;
        threadData[i].num_threads = numthreads;
        threadData[i].barrier = &barrier;

        if (pthread_create(&threads[i], NULL, prefixSum, (void *)&threadData[i]))
        {
            perror("pthread_create failed");
            free(prefixScan);
            free(sums);
            return NULL;
        }
    }

    for (int i = 0; i < numthreads; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            perror("pthread_join failed");
            free(prefixScan);
            free(sums);
            return NULL;
        }
    }

    pthread_barrier_destroy(&barrier);
    free(sums);

    return prefixScan;
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
    FILE *output = fopen(argv[4], "w");

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
