#ifndef __myalloc_h__
#define __myalloc_h__

#define ERR_OUT_OF_MEMORY  (-1)
#define ERR_BAD_ARGUMENTS  (-2)
#define ERR_SYSCALL_FAILED (-3)
#define ERR_CALL_FAILED    (-4)
#define ERR_UNINITIALIZED   (-5)

#define MAX_ARENA_SIZE (0x7FFFFFFF)

extern int statusno;

extern int myinit(size_t size);
extern int mydestroy();

extern void* myalloc(size_t size);
extern void myfree(void *ptr);

//Note: size represents the number of bytes available for allocation and does
//not include the header bytes. 
typedef struct __node_t {
  size_t size; 
  unsigned short is_free;
  struct __node_t *fwd;
  struct __node_t *bwd;
} node_t;




#endif
