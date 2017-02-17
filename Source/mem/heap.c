#include "../system.h"

//used to determine whether a new block should be of size 1 or 2
/*uint32_t chunk_size_1_requested = 0;
uint32_t chunk_size_2_requested = 0;
uint32_t total_chunks_requested = 0;*/

uint32_t chunks_allocated = 0;

heap_node_t* heap_list;

int heap_init()
{
  printf("size of heap node is %d bytes\n", sizeof(heap_node_t));
  heap_list = kmalloc(sizeof(heap_node_t));
  heap_list->next = NULL:
  return 1;
}

uint32_t* heap_alloc(uint32_t length)
{
  return NULL;
}

int heap_free(uint32_t* addr, uint32_t length)
{
  return 1;
}
