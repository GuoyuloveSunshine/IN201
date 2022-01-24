#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define HEAPSIZE (16384)

#define NUMBER_BLOCKS_TEST (4096)
#define MAX_BLOCK_SIZE (256)
#define DEFAULT_NUMBER_ITERATIONS (1000)

uint8_t heap[HEAPSIZE];

void* memalloc(unsigned int size);
void  memfree(void* p);



                /***********************************
                 * trivial allocation with no free *
                 ***********************************/

#ifdef TRIVIAL
/*
 * at any time, the first part of the heap is allocated and the
 * remaining part is free
 * free_zone_start is the index of the first unallocated cell
 */
unsigned int free_zone_start = 0;

/*
 * to allocate, simply increase the limit between allocated and
 * unallocated memory
 */
void* memalloc(unsigned int size)
{
  free_zone_start += size;

  // if there is not enough memory available in the heap,
  // return an error
  if (free_zone_start >= HEAPSIZE) {
    printf("Error: out of memory.\n");
    return NULL;
  } else {
    return heap + free_zone_start - size;
  }
}

/*
 * do nothing
 */
void memfree(void* p) {}
#endif


                /**************************************
                 * simple first-fit allocation with   *
                 * a linked list for free blocks      *
                 **************************************/

/*
 * list of free zones
 */
struct free_list {
  unsigned int size;
  struct free_list* next;
};

/*
 * contains only the size of the corresponding block,
 * but use a struct for modularity (if we later want to add more metadata)
 */
struct block_header {
  unsigned int size;
};

/*
 * free zone metadata is stored at the beginning of the zone
 * initially the entire heap (minus metadata) is one free block
 */
struct free_list* free_list;

/*
 * initialize the free list: the whole heap is a single free block
 */
void meminit()
{
  free_list = (struct free_list*) heap;
  free_list->size = HEAPSIZE - sizeof(struct free_list);
  free_list->next = NULL;
  return;
}

#ifndef TRIVIAL
/*
 * allocate a new block on the heap, first fit
 */
void* memalloc(unsigned int size)
{
  struct free_list* b;
  void* base;

  // allocated blocks must be large enough to store a free_list header
  // upon deallocation
  if (size < sizeof(struct free_list)) {
    size = sizeof(struct free_list);
  }

  //look for a block large enough in the free_list
  for (b = free_list ; b != NULL ; b = b->next) {
    if (b->size >= size + sizeof(struct block_header)) {

      // compute remaining free space
      b->size -= size + sizeof(struct block_header);

      // pointer to the base of the allocated block, including header
      base = (uint8_t*) b + sizeof(struct free_list) + b->size;

      // record the new block's size in its header
      ((struct block_header*) base)->size = size;

      printf("Allocating block at offset %ld\n", (uint8_t*) base - heap);

      // return a pointer to the new block
      return ((uint8_t*) base + sizeof(struct block_header));
    }
  }

  // end of the free_list was reached without finding a large enough block
  // return an error
  printf("Error: no suitable block found for allocation.\n");
  return NULL;
}
#endif

#ifdef NOCOALESCING
/*
 * provided p points to the start of a block, deallocate this block
 * simple version (no coalescing)
 */
void memfree(void* p)
{
   struct free_list* base;

  // compute pointer to allocated block header
  base = (struct free_list*) ((uint8_t*) p - sizeof(struct block_header));

  printf("Deallocating block at offset %ld\n", (uint8_t*) base - (uint8_t*) heap);

  // compute free zone size
  base->size = ((struct block_header*) base)->size
    + sizeof(struct block_header)
    - sizeof(struct free_list);

  // add new free zone to the free_list
  base->next = free_list ;
  free_list = base;
  return;
}
#endif

                /**************
                 * coalescing *
                 **************/

#ifdef COALESCING
 /*
  * look for a block adjacent to b in the free list
  * can only find a block with a lower address
  * return NULL if not found
  */
struct free_list* find_adj_free(struct block_header* b)
{
  struct free_list* p = free_list;

  while (p != NULL) {
    if ((char*) p + sizeof(struct free_list) + p->size == (char*) b) {
      break;
    } else {
      p = p->next;
    }
  }
  return p;
}

/*
 * provided p points to the start of a block, deallocate this block
 * version with coalescing
 */
void memfree(void* p)
{
  struct free_list* adj;
  struct free_list* base = (struct free_list*)
    ((uint8_t*) p - sizeof(struct block_header));

  printf("Deallocating block at offset %ld\n",
         (uint8_t*) base - (uint8_t*) heap);

  // merge with adjacent block if there is one
  adj = find_adj_free((struct block_header*) base);
  if (adj != NULL) {
    adj->size += ((struct block_header*) base)->size +
      sizeof(struct block_header);
    printf("Debug: merging adjacent free blocks\n");
  } else {
    base->size = ((struct block_header*) base)->size
      + sizeof(struct block_header)
      - sizeof(struct free_list);
    base->next = free_list ;

    free_list = base;
  }

  return;
}
#endif

int main(int argc, char** argv)
{
  void* blocks[NUMBER_BLOCKS_TEST];
  unsigned int i,j,size,iterations;

  if (argc > 1) {
    iterations = atoi(argv[1]);
  } else {
    iterations = DEFAULT_NUMBER_ITERATIONS;
  }

  // seed the PRNG with current process PID
  srand(getpid());

#ifndef TRIVIAL
  // initialize the heap
  meminit();
#endif

  // initialize all pointers to NULL
  for (i = 0 ; i < NUMBER_BLOCKS_TEST ; i++) {
    blocks[i] = NULL;
  }

  for(i = 0 ; i < iterations ; i++) {
    // alloc or free of write
    switch (rand() % 3) {
    case 0:
      // allocate a block at random
      size = (rand() % MAX_BLOCK_SIZE);
      for(j = 0 ; j < NUMBER_BLOCKS_TEST && blocks[j] != NULL ; j++);
      printf("Debug: trying to allocate block %u with size %u...\n", j, size);
      if (j < NUMBER_BLOCKS_TEST) {
        blocks[j] = memalloc(size);
      }
      break;
    case 1:
      // write in an allocated block
      for(j = 0 ; j < NUMBER_BLOCKS_TEST && blocks[j] == NULL ; j++);
      if (j < NUMBER_BLOCKS_TEST ) {
        printf("Debug: trying to write in block %u...\n", j);
        *((uint8_t*) blocks[j]) = rand() % 256;
      }
      break;
    default:
      // deallocate an allocated block
      for(j = 0 ; j < NUMBER_BLOCKS_TEST && blocks[j] == NULL ; j++);
      if (j < NUMBER_BLOCKS_TEST) {
        printf("Debug: trying to deallocate block %u...\n", j);
        memfree(blocks[j]);
        blocks[j] = NULL;
      }
    }
  }

  return 0;
}