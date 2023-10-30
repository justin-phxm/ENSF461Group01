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
    fprintf(stderr, "...header size is 32 bytes\n");

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
    if (size > arenaSize - sizeof(node_t))
    {
        fprintf(stderr, "...no such free chunk exists\n");
        fprintf(stderr, "...setting error code\n");
        statusno = ERR_OUT_OF_MEMORY;
        return NULL;
    }
    fprintf(stderr, "...found free chunk of %lu bytes with header at %p\n", size, _arena_start);
    fprintf(stderr, "...free chunk->fwd currently points to (%p)\n", ((node_t *)_arena_start)->fwd);
    fprintf(stderr, "...free chunk->bwd currently points to (%p)\n", ((node_t *)_arena_start)->bwd);
    fprintf(stderr, "...checking if splitting is required\n");
    // todo: check if splitting is required
    if (1)
    {
        fprintf(stderr, "...splitting is not required\n");
    }
    fprintf(stderr, "...updating chunk header at %p\n", _arena_start);
    fprintf(stderr, "...being careful with my pointer arithmetic and void pointer casting\n");
    fprintf(stderr, "...allocation starts at %p\n", _arena_start + sizeof(node_t));
    return _arena_start;
}

void myfree(void *ptr)
{
    return;
}