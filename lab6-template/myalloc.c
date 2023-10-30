#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "myalloc.h"
void *_arena_start = NULL;
size_t arenaSize = 0;
int statusno = 0;

int myRound(double x)
{
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}

int myinit(size_t size)
{
    fprintf(stderr, "Initializing arena: \n");
    fprintf(stderr, "...requested size %lu bytes\n", size);
    arenaSize = size;
    if (size > MAX_ARENA_SIZE)
    {
        fprintf(stderr, "...error: requested size larger than MAX_ARENA_SIZE (%d)\n", MAX_ARENA_SIZE);
        return ERR_BAD_ARGUMENTS;
    }
    if (size <= 0)
    {
        fprintf(stderr, "...error: requested size must be greater than 0\n");
        return ERR_BAD_ARGUMENTS;
    }
    fprintf(stderr, "...pagesize is %d bytes\n", getpagesize());
    if (size != 4096)
    {
        fprintf(stderr, "...adjusting size with page boundaries\n");

        double arenaLength = (double)size / (double)getpagesize();
        int arenaUnits = myRound(arenaLength);

        if (arenaUnits == 0)
        {
            arenaUnits = 1;
        }

        arenaSize = arenaUnits * getpagesize();
        fprintf(stderr, "...adjusted size is %ld bytes\n", arenaSize);
    }
    fprintf(stderr, "...mapping arena with mmap()\n");

    _arena_start = mmap(NULL, arenaSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (_arena_start == MAP_FAILED)
    {
        fprintf(stderr, "...error: mmap() failed\n");
        return ERR_BAD_ARGUMENTS;
    }

    fprintf(stderr, "...arena starts at %p\n", _arena_start);
    fprintf(stderr, "...arena ends at %p\n", _arena_start + arenaSize);
    fprintf(stderr, "...initializing header for initial free chunk\n");
    // todo: initialize header for initial free chunk
    node_t *header = (node_t *)_arena_start;
    header->size = arenaSize - sizeof(node_t);
    header->is_free = 1;
    header->fwd = NULL;
    header->bwd = NULL;
    fprintf(stderr, "...header size is %ld bytes\n", sizeof(node_t));

    return arenaSize;
}

int mydestroy()
{
    fprintf(stderr, "Destroying arena:\n");
    // munmap returns -1 on failure.
    if (munmap(_arena_start, arenaSize) != 0)
    {
        fprintf(stderr, "...error: cannot destory uninitializd arena. Setting error status\n");
        statusno = ERR_UNINITIALIZED;
        return ERR_UNINITIALIZED;
    }
    fprintf(stderr, "...unmapping arena with munmap()\n");

    return 0;
}

void *myalloc(size_t size)
{
    if (_arena_start == NULL)
    {
        fprintf(stderr, "Error: Unintialized. Setting status code\n");
        statusno = ERR_UNINITIALIZED;
        return NULL;
    }
    fprintf(stderr, "Allocating memory:\n");
    fprintf(stderr, "...looking for free chunk of >= %lu bytes\n", size);
    node_t *header = (node_t *)_arena_start;
    // Move the header pointer to the first free chunk of memory that is large enough.
    while ((header->is_free == 0 || header->size < size))
    {
        if (header->fwd == NULL)
        {
            fprintf(stderr, "...no such free chunk exists\n");
            fprintf(stderr, "...setting error code\n");
            statusno = ERR_OUT_OF_MEMORY;
            return NULL;
        }
        header = header->fwd;
    }
    if (header->size < size)
    {
        fprintf(stderr, "...no such free chunk exists\n");
        fprintf(stderr, "...setting error code\n");
        statusno = ERR_OUT_OF_MEMORY;
        return NULL;
    }
    size_t chunkSize = header->size;
    fprintf(stderr, "...found free chunk of %lu bytes with header at %p\n", header->size, header);
    fprintf(stderr, "...free chunk->fwd currently points to %p\n", header->fwd);
    fprintf(stderr, "...free chunk->bwd currently points to %p\n", header->bwd);
    fprintf(stderr, "...checking if splitting is required\n");
    // todo: check if splitting is required
    if (header->size > size + sizeof(node_t))
    {
        fprintf(stderr, "...splitting free chunk\n");
    }
    else if (header->size > size)
    {
        fprintf(stderr, "...splitting not possible\n");
    }
    else
    {
        fprintf(stderr, "...splitting is not required\n");
    }
    fprintf(stderr, "...updating chunk header at %p\n", header);
    header->is_free = 0;
    header->size = size;
    header->fwd = NULL; // might not need this line...
    // if there is space remaining in the arena for another node_t, set the fwd pointer to the next node_t
    // Calculate the address of the potential next header
    intptr_t nextHeaderAddress = (intptr_t)header + size + sizeof(node_t);
    // Check if there's enough space for another node_t
    if (nextHeaderAddress + sizeof(node_t) < (intptr_t)_arena_start + arenaSize)
    {
        header->fwd = (node_t *)nextHeaderAddress;
        node_t *next = header->fwd;
        next->size = chunkSize - size - sizeof(node_t);
        next->is_free = 1;
        next->fwd = NULL; // might not need this line...
        next->bwd = header;
    }
    else
    {
        // Fill up the remaining space with the header
        header->size = chunkSize;
    }

    fprintf(stderr, "...being careful with my pointer arithmetic and void pointer casting\n");
    node_t *allocationStart = (node_t *)((void *)header + sizeof(node_t));
    fprintf(stderr, "...allocation starts at %p\n", allocationStart);

    return allocationStart;
}

void myfree(void *ptr)
{
    fprintf(stderr, "Freeing allocated memory:\n");
    fprintf(stderr, "...supplied pointer %p\n", ptr);
    fprintf(stderr, "...being careful with my pointer arithmetic and void pointer casting\n");
    node_t *header = (node_t *)((void *)ptr - sizeof(node_t));
    fprintf(stderr, "...accessing chunk header at %p\n", header);
    fprintf(stderr, "...chunk of size %ld\n", header->size);
    fprintf(stderr, "...checking if coalescing is needed\n");
    // todo: check if coalescing is needed
    if (1)
    {
        fprintf(stderr, "...coalescing is not needed\n");
    }
    header->is_free = 1;

    return;
}