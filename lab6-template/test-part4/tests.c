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

//PRINTF_GREEN("Assert %d passed!\n", test++);

void test_allocationfree_placement(){
  int test = 1;
  int size = 0;
  int page_size = getpagesize();
  void *buff, *buff2,  *buff3;
  node_t *header = NULL;
 
  PRINTF_GREEN(">> Testing basic allocation free placement. (test_allocationfree_placement)\n");

  // Test: Create two allocatations.
  // Free first allocation but leave the second allocation in place. 
  // Create third allocation an Ensure that the 
  // third allocation is placed in the first free chunk 
  myinit(page_size);

  size = 32;
  buff = myalloc(size); 
  assert(buff != NULL);  
  PRINTF_GREEN("Assert %d passed!\n", test++);

  memset(buff, 'a', size);

  header = (node_t *)(buff - sizeof(node_t));

  //This checks that the header for the first allocation is placed
  //at the start of a page boundary. If this check fails, then the 
  //first header is not placed at the start of the arena.
  assert(((unsigned long)header & 0xFFF) == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  print_header(header);

  buff2 = myalloc(size);
  //Do something with buff2 to avoid compiler warnings
  if(buff2){}

  myfree(buff); 

  //Next allocation should be in same place as the first allocation
  buff3 = myalloc(size);
  assert(buff3 == buff);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  //The characters from the previous memset should also be there 
  assert(((char *)buff)[0] == 'a' && ((char *)buff)[size-1] == 'a');
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}




int main() {
  test_allocationfree_placement();
}
