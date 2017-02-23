#include <system.h>

//used to determine whether a new block should be of size 1 or 2
/*uint32_t chunk_size_1_requested = 0;
uint32_t chunk_size_2_requested = 0;
uint32_t total_chunks_requested = 0;*/

uint32_t chunks_allocated = 0;

heap_node_t* heap_list;

int heap_init()
{
  printf("size of heap node is %d bytes\n", sizeof(heap_node_t));
  heap_list = (heap_node_t*)kmalloc(sizeof(heap_node_t));
  heap_list->next = NULL;
  
  return 1;
}

uint32_t* heap_alloc(uint32_t length)
{
  //first check to ensure that the amount of memory requested is able to be supplied. Ideally, if more than 3KB is needed,
  //kmalloc should be used
  //...
  
  heap_node_t* node = heap_list;
  
  bool found_free_node = false;
  while(node->next != NULL)
  {
    if (node->memory_allocated < HEAP_CHUNK_DATA_SIZE)
    {
      if (HEAP_CHUNK_DATA_SIZE - node->memory_allocated > length)
      {
        bool success = false;
        uint32_t return_value = heap_allocate_from_node(node, length, &success);
        if (success == true)
          return return_value;
      }      
    }
    node = node->next;
  }
  
  //if we got to here, then no free chunks were found of the correct size. IN order to fix this, 
  //add another node to the list, and select it. Then call heap_allocate_from_node()
  
  return NULL;
}

int heap_allocate_from_node(heap_node_t* node, int length)
{
  
}

int heap_free(uint32_t* addr, uint32_t length)
{
  return 1;
}
