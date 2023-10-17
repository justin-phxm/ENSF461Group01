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

// PRINTF_GREEN("Assert %d passed!\n", test++);

void test_free_basic(){
  int size, test=1;
  int page_size = getpagesize();
  void *buff;
  node_t *header; 

  PRINTF_GREEN(">>Testing frees without coalescing. (test_free_basic)\n");
  myinit(page_size);
  size = page_size - sizeof(node_t);
  buff = myalloc(size); 
  header = (node_t *)(buff - sizeof(node_t));

  assert(header->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  myfree(buff);

  assert(header->is_free == 1);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  assert(header->size == size);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


int main() {
  test_free_basic();
}
