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


void test_allocation_withsplits(){
  int test=1;
  int size =0;
  int page_size = getpagesize();
  void *buff = NULL, *buff2 = NULL;
  node_t *header = NULL, *header2 = NULL; 

  PRINTF_GREEN(">>Testing allocations with splits.\n");

  // Test: First allocation causes split. Check that the header is in the
  // correct place, has the correct field values, and that chunks were split
  // correctly 
  myinit(page_size);
  buff = myalloc(64);
  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  header = (node_t *)(buff - sizeof(node_t));
  print_header(header);

  assert(header->size == 64); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->bwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->fwd == ((void*)header)+sizeof(node_t)+64);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  node_t *next = header->fwd;
  print_header(next);

  assert(next->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(next->bwd == header);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(next->size == page_size-64-(sizeof(node_t)*2));
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(next->is_free == 1);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  // Test: Second allocation uses up remaining free space. Check that 
  // the allocation is in the correct place and has the correct field values.
  size = page_size - 64 - (sizeof(node_t) * 2);
  buff2 = myalloc(size);

  header2 = (node_t *)(buff2 - sizeof(node_t));
  print_header(header2);

  assert(header2 == next);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->size == size);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->bwd == header);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();

  // Test: Only split if the process of splitting would leaves 
  // enough room for another chunk.
  myinit(page_size);
  buff = myalloc(64);
  //This should leave 10 bytes remaining in the arena
  size = page_size - 64 - (sizeof(node_t) * 2) - 10;
  buff2 = myalloc(size);

  header2 = (node_t *)(buff2 - sizeof(node_t));
  print_header(header2);

  assert(header2->size == size + 10);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


int main() {
  test_allocation_withsplits();
}
