#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#define TRUE 1
#define FALSE 0
#define TLB_SIZE 8

// Forward declarations of structs
struct TLB_entry;
struct Page_table_entry;
struct Page_table;

// TLB structure
typedef struct
{
    unsigned int vpn;
    unsigned int ppn;
    int valid; // validity bit: can be 0 (invalid) or 1 (valid)
} TLB_entry;

typedef struct
{
    // unsigned int vpn;
    unsigned int pfn;
    int valid; // validity bit: can be 0 (invalid) or 1 (valid)
    unsigned long int r1, r2;
} Page_table_entry;

typedef struct
{
    Page_table_entry entries[4];
} Page_table;

// Output file
FILE *output_file;

// TLB replacement strategy (FIFO or LRU)
char *strategy;

// Global variables
unsigned int size_offset, size_ppn, size_vpn, current_pid = 0;
unsigned long int **physical_memory = NULL;
unsigned long int r1 = 0, r2 = 0;
TLB_entry TLB[TLB_SIZE];
Page_table pt;

// Function prototypes
void initialize_TLB();
void define(unsigned int num_offset_bits, unsigned int num_ppn_bits, unsigned int num_vpn_bits);
void ctxswitch(unsigned int pid);
void map(unsigned int vpn, unsigned int ppn);
void unmap(unsigned int vpn);
void pinspect(unsigned int vpn);
TLB_entry tinspect(unsigned int tlbn);
void load(char *dst, unsigned int src);
void add();

// unsigned long int linspect();
void initialize_TLB();
void initialize_physical_memory(unsigned int num_offset_bits, unsigned int num_ppn_bits);
void cleanup_physical_memory();

char **tokenize_input(char *input)
{
    char **tokens = NULL;
    char *token = strtok(input, " ");
    int num_tokens = 0;

    while (token != NULL)
    {
        num_tokens++;
        tokens = realloc(tokens, num_tokens * sizeof(char *));
        tokens[num_tokens - 1] = malloc(strlen(token) + 1);
        strcpy(tokens[num_tokens - 1], token);
        token = strtok(NULL, " ");
    }

    num_tokens++;
    tokens = realloc(tokens, num_tokens * sizeof(char *));
    tokens[num_tokens - 1] = NULL;

    return tokens;
}

int main(int argc, char *argv[])
{
    const char usage[] = "Usage: memsym.out <strategy> <input trace> <output trace>\n";
    char *input_trace;
    char *output_trace;
    char buffer[1024];

    // Parse command line arguments
    if (argc != 4)
    {
        printf("%s", usage);
        return 1;
    }
    strategy = argv[1];
    input_trace = argv[2];
    output_trace = argv[3];

    // Open input and output files
    FILE *input_file = fopen(input_trace, "r");
    output_file = fopen(output_trace, "w");

    while (!feof(input_file))
    {
        // Read input file line by line

        /*
        buffer will look like
        % Test 1.2: define
        define 4 6 6

        */
        char *rez = fgets(buffer, sizeof(buffer), input_file);
        if (!rez)
        {
            fprintf(stderr, "Reached end of trace. Exiting...\n");
            return -1;
        }
        else
        {
            // Remove endline character
            buffer[strlen(buffer) - 1] = '\0';
        }
        char **tokens = tokenize_input(buffer);
        /* tokens will look like:
        tokens[0] = %
        tokens[1] = Test
        tokens[2] = 1.2:

        a[0] = define
        a[1] = 4
        a[2] = 6
        a[3] = 6
        */
        // TODO: Implement your memory simulator
        // for example, 1.2.in is 4 6 6
        if (strcmp(tokens[0], "%") != 0)
        {
            if (strcmp(tokens[0], "ctxswitch") != 0)
            {
                fprintf(output_file, "Current PID: %d. ", current_pid);
            }
            if (size_offset == 0 && size_ppn == 0 && size_vpn == 0)
            {
                if (strcmp(tokens[0], "define") == 0)
                {
                    define(atoi(tokens[1]), atoi(tokens[2]), atoi(tokens[3]));
                }
                else
                {
                    fprintf(output_file, "Error: attempt to execute instruction before define\n");
                    return -1;
                }
            }
            else
            {

                if (strcmp(tokens[0], "define") == 0)
                {
                    fprintf(output_file, "Error: multiple calls to define in the same trace\n");
                    return -1;
                }
                else if (strcmp(tokens[0], "ctxswitch") == 0)
                {
                    ctxswitch(atoi(tokens[1]));
                }
                else if (strcmp(tokens[0], "load") == 0)
                {
                    char *skipHash = tokens[2] + 1;
                    load(tokens[1], atoi(skipHash));
                }
                else if (strcmp(tokens[0], "add") == 0)
                {
                    add();
                }

                else
                {
                    fprintf(output_file, "Error: Unknown instruction...\n");
                    return -1;
                }
            }
        }

        // Deallocate tokens
        for (int i = 0; tokens[i] != NULL; i++)
        {
            // fprintf(output_file, "%s\n", tokens[i]);
            free(tokens[i]);
        }
        free(tokens);
    }

    // Close input and output files
    fclose(input_file);
    fclose(output_file);
    cleanup_physical_memory(size_offset + size_ppn);

    return 0;
}

void load(char *dst, unsigned int src)
{
    if (strcmp(dst, "r1") == 0)
    {
        r1 = src;
        fprintf(output_file, "Loaded immediate %d into register %s\n", src, dst);
    }
    else if (strcmp(dst, "r2") == 0)
    {
        r2 = src;
        fprintf(output_file, "Loaded immediate %d into register %s\n", src, dst);
    }
    else
    {
        fprintf(output_file, "Error: invalid register operand %s\n", dst);
    }
}

// Initialize TLB entries
void initialize_TLB()
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        TLB[i].vpn = 0;
        TLB[i].ppn = 0;
        TLB[i].valid = 0;
    }
}

// Initialize physical_memory
void initialize_physical_memory(unsigned int num_offset_bits, unsigned int num_ppn_bits)
{
    fprintf(output_file, "Memory instantiation complete. ");
    int numPages = pow(2, num_ppn_bits);
    int pageSize = pow(2, num_offset_bits);

    physical_memory = (unsigned long int **)malloc(numPages * sizeof(unsigned long int *));
    if (physical_memory == NULL)
    {
        fprintf(stderr, "Memory allocation failed. Exiting.\n");
        exit(1);
    }

    // Initialize each word in memory to 0
    for (int i = 0; i < num_ppn_bits; i++)
    {
        physical_memory[i] = (unsigned long int *)calloc(pageSize, sizeof(unsigned long int));
        if (physical_memory[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed for element %d. Exiting.\n", i);
            exit(1);
        }
    }
}

// Initialize Page Table
void initialize_Page_table()
{
    for (int i = 0; i < 4; i++)
    {
        pt.entries[i].pfn = 0;
        pt.entries[i].valid = 0;
    }
}

// Cleanup function to free the allocated memory
void cleanup_physical_memory(int size)
{
    for (int i = 0; i < size; i++)
    {
        free(physical_memory[i]);
    }
    free(physical_memory);
}
// Function to set global variables
void define(unsigned int num_offset_bits, unsigned int num_ppn_bits, unsigned int num_vpn_bits)
{
    // fprintf(output_file, "Memory instantiation started. ");
    initialize_physical_memory(num_offset_bits, num_ppn_bits);
    fprintf(output_file, "OFF bits: %d. PFN bits: %d. VPN bits: %d\n", num_offset_bits, num_ppn_bits, num_vpn_bits);
    initialize_TLB();
    initialize_Page_table();

    size_offset = num_offset_bits;
    size_ppn = num_ppn_bits;
    size_vpn = num_vpn_bits;
}

void ctxswitch(unsigned int pid)
{
    // Check if pid is withing bounds
    if (pid < 0 || pid > 3)
    {
        fprintf(output_file, "Current PID: %d. Invalid context switch to process %d\n", current_pid, pid);
        return;
    }
    // Save contents from old process
    pt.entries[current_pid].r1 = r1;
    pt.entries[current_pid].r2 = r2;

    // Load contents from new process
    current_pid = pid;
    r1 = pt.entries[pid].r1;
    r2 = pt.entries[pid].r2;
    fprintf(output_file, "Current PID: %d. Switched execution context to process: %d\n", pid, pid);
}

void add()
{
    unsigned long int sum = r1 + r2;
    fprintf(output_file, "Added contents of registers r1 (%ld) and r2 (%ld). Result: %ld\n", r1, r2, sum);
    r1 = sum;
}