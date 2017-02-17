#ifndef HEAP_H
#define HEAP_H

#define HEAP_CHUNK_SIZE_1       256
#define HEAP_CHUNK_SIZE_2       64

#define HEAP_CHUNK_HEADER_SIZE  8 //space for pointer to next struct, and integer for amount of space left in chunk
#define HEAP_CHUNK_DATA_SIZE    (((4096-HEAP_CHUNK_HEADER_SIZE)*8)/9)
#define HEAP_CHUNK_BITMAP_SIZE  (HEAP_CHUNK_DATA_SIZE/8)

typedef struct heap_node {
  uint32_t memory_allocated;
  struct heap_node* next;
  //after these 8 bytes, the rest is the bitap and data
  uint8_t heap_chunk_bitmap[HEAP_CHUNK_BITMAP_SIZE];
  //data....  
}  __attribute__((packed)) heap_node_t;//must be packed, in order to fit in 4096 bytes

#endif
