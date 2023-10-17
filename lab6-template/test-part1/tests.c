#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include "myalloc.h"

#define PRINTF_GREEN(...) fprintf(stderr, "\033[32m"); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\033[0m");


void print_header(node_t *header){
  //Note: These printf statements may produce a segmentation fault if the buff
  //pointer is incorrect, e.g., if buff points to the start of the arena.
  printf("Header->size: %lu\n", header->size);
  printf("Header->fwd: %p\n", header->fwd);
  printf("Header->bwd: %p\n", header->bwd);
  printf("Header->is_free: %d\n", header->is_free);
}


void test_init_destroy(){
  int arena_size = 0, test = 1; 
  int page_size = getpagesize();
  
  PRINTF_GREEN(">> Part 1: Testing initialization. (test_init_destroy)\n");

  // The myinit function must adjust the arena size to be a multiple of pagesize.  
  arena_size = myinit(1);

  assert(arena_size == page_size);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  assert(mydestroy() == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  arena_size = myinit((page_size*2)-10);

  assert(arena_size == 2*page_size);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  assert(mydestroy() == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  arena_size = myinit(page_size);

  assert(arena_size == page_size);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  assert(mydestroy() == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  // The myinit function must also check for error conditions and bad arguments.
  assert(myinit(-1) == ERR_BAD_ARGUMENTS); 
  PRINTF_GREEN("Assert %d passed!\n", test++);

  assert(mydestroy() == ERR_UNINITIALIZED);
  PRINTF_GREEN("Assert %d passed!\n", test++);
}


int main() {
  test_init_destroy();
}
