#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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



void test_free_coalescing_case3(){
  int test=1;
  int size, size2;
  int page_size = getpagesize();
  void *buff, *buff2;
  node_t *header, *header2; 

  PRINTF_GREEN(">>Testing frees with coalescing. Case 3.\n");

  myinit(page_size);

  size=64;
  //Have this allocation fill up the rest of the arena
  size2=page_size - size - sizeof(node_t)*2;
 
  buff = myalloc(size);
  buff2 = myalloc(size2);

  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff2 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  header = ((void*)buff) - sizeof(node_t);
  header2 = ((void*)buff2) - sizeof(node_t);

  print_header(header);
  print_header(header2);

  assert(header->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  myfree(buff2);
  assert(header2->is_free == 1);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->size == size2);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  //this should cause coalescing. 
  myfree(buff); 

  //The coalesced node header should reside at the start of the 
  //arena (i.e., where header points)
  print_header(header);

  assert(header->size == page_size - sizeof(node_t)); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->bwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


void test_free_coalescing_case2(){
  int test=1;
  int size, size2;
  int page_size = getpagesize();
  void *buff, *buff2;
  node_t *header, *header2; 

  PRINTF_GREEN(">>Testing frees with coalescing. Case 2.\n");

  myinit(page_size);

  size=64;
  //Have this allocation fill up the rest of the arena
  size2=page_size - size - sizeof(node_t)*2;
 
  buff = myalloc(size);
  buff2 = myalloc(size2);

  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff2 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  header = ((void*)buff) - sizeof(node_t);
  header2 = ((void*)buff2) - sizeof(node_t);

  print_header(header);
  print_header(header2);

  assert(header->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  myfree(buff); 
  assert(header->is_free == 1);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->size == size);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  //this should cause coalescing. 
  myfree(buff2);

  //The coalesced node header should reside at the start of the 
  //arena (i.e., where header points)
  print_header(header);

  assert(header->size == page_size - sizeof(node_t)); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->bwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


void test_free_coalescing_case1(){
  int test=1;
  int size, size2, size3;
  int page_size = getpagesize();
  void *buff, *buff2, *buff3;
  node_t *header, *header2, *header3; 

  PRINTF_GREEN(">>Testing frees with coalescing. Case 1.\n");

  myinit(page_size);

  size=64;
  size2=128;
  //Have the third allocation fill up the rest of the arena
  size3=page_size - size - size2 - sizeof(node_t)*3;
  
  //Case 1: Combine prev, current, next
  buff = myalloc(size);
  buff2 = myalloc(size2);
  buff3 = myalloc(size3);

  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff2 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff3 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  
  header = ((void*)buff) - sizeof(node_t);
  header2 = ((void*)buff2) - sizeof(node_t);
  header3 = ((void*)buff3) - sizeof(node_t);

  print_header(header);
  print_header(header2);
  print_header(header3);

  assert(header->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header2->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header3->is_free == 0);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  myfree(buff); 
  myfree(buff3);

  assert(header->is_free == 1);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header3->is_free == 1);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  assert(header->size == size);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header3->size == size3);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  //this should cause coalescing. 
  myfree(buff2);
 
  //The coalesced node header should reside at the start of the 
  //arena (i.e., where header points)
  print_header(header);

  assert(header->size == page_size - sizeof(node_t)); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->bwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


void test_free_coalescing_chains_fwd(){
  int test=1;
  int size;
  int page_size = getpagesize();
  void *buff, *buff2, *buff3, *buff4;

  PRINTF_GREEN(">>Testing frees with coalescing chaining\n");

  myinit(page_size);

  size=64;
 
  buff = myalloc(size);
  buff2 = myalloc(size);
  buff3 = myalloc(size);
  buff4 = myalloc(size);

  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff2 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff3 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff4 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  node_t *header = ((void*)buff) - sizeof(node_t);

  myfree(buff);
  myfree(buff2);
  myfree(buff3);
  myfree(buff4);

  print_header(header);

  //check to make sure the above seq. ends with 
  //a single free node.
  assert(header->size == page_size - sizeof(node_t)); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->is_free == 1); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->bwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


void test_free_coalescing_chains_bwd(){
  int test=1;
  int size;
  int page_size = getpagesize();
  void *buff, *buff2, *buff3, *buff4;

  PRINTF_GREEN(">>Testing frees with coalescing chaining\n");

  myinit(page_size);

  size=64;
 
  buff = myalloc(size);
  buff2 = myalloc(size);
  buff3 = myalloc(size);
  buff4 = myalloc(size);

  assert(buff != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff2 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff3 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(buff4 != NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  node_t *header = ((void*)buff) - sizeof(node_t);

  myfree(buff4);
  myfree(buff3);
  myfree(buff2);
  myfree(buff);

  print_header(header);

  //check to make sure the above seq. ends with 
  //a single free node.
  assert(header->size == page_size - sizeof(node_t)); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->is_free == 1); 
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->bwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);
  assert(header->fwd == NULL);
  PRINTF_GREEN("Assert %d passed!\n", test++);

  mydestroy();
}


int main(int argc, char **argv) {
  int test = atoi(argv[1]);

  switch(test) {
    case 1:
      test_free_coalescing_case1();
      break;
    case 2:
      test_free_coalescing_case2();
      break;
    case 3:
      test_free_coalescing_case3();
      break;
    case 4:
      test_free_coalescing_chains_fwd();
      break;
    case 5:
      test_free_coalescing_chains_bwd();
      break;
  }

}
