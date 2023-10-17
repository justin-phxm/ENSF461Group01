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

void test_allocation_basic(){
  int size = 0, test = 1;
  int page_size = getpagesize();
  void *buff, *buff2 = NULL;
 
  PRINTF_GREEN(">> Testing allocations without the possibility to split. No Frees. (test_allocation_basic)\n");

  buff = myalloc(page_size);
  assert(statusno == ERR_UNINITIALIZED);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  // Allocation not possible because we didn't account for the header which is
  // also placed in the arena and takes of sizeof(node_t) bytes. 
  myinit(page_size);
  buff = myalloc(page_size); 
  assert(statusno == ERR_OUT_OF_MEMORY);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  size = page_size-sizeof(node_t);
  buff = myalloc(size); 
  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  // Check that we can write to the allocated memory  
  memset(buff, 'a', size);
  assert(((char *)buff)[0] == 'a' && ((char *)buff)[size-1] == 'a');
  PRINTF_GREEN("Assert %d passed!\n", test++);

  //This allocation should fail because the previous allocation used all of
  //the remaining space. 
  buff2 = myalloc(1); 
  assert(buff2 == NULL);  
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(statusno == ERR_OUT_OF_MEMORY);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


int main() {
  test_allocation_basic();
}
