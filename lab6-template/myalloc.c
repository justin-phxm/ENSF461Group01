#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "myalloc.h"

void *_arena_start = NULL;
size_t arenaSize = 0;

int myinit(size_t size)
{
    _arena_start = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    arenaSize = size;
    if (_arena_start == MAP_FAILED)
    {
        return ERR_BAD_ARGUMENTS;
    }
    return getpagesize();
}

int mydestroy()
{
    if (munmap(_arena_start, arenaSize))
    {
        return ERR_UNINITIALIZED;
    }
    return 0;
}

void *myalloc(size_t size)
{
    if (_arena_start == NULL)
    {
        statusno = ERR_UNINITIALIZED;
        return;
    }
    return NULL;
}

void myfree(void *ptr)
{
    return;
}