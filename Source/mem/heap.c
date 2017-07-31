#include <system.h>

//used to determine whether a new block should be of size 1 or 2
/*uint32_t chunk_size_1_requested = 0;
uint32_t chunk_size_2_requested = 0;
uint32_t total_chunks_requested = 0;*/

uint32_t chunks_allocated = 0;

heap_node_t* heap_list;

int heap_init()
{
  printf("heap_init\n");
  printf("....size of heap node is %d bytes\n", sizeof(heap_node_t));
  printf("....data+bitmap: %d\n", HEAP_CHUNK_HEADER_SIZE+HEAP_CHUNK_BITMAP_SIZE+HEAP_CHUNK_DATA_SIZE);
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
  while(node->next != NULL) //loop through nodes
  {
    if (node->memory_allocated < HEAP_CHUNK_DATA_SIZE) //check if there is any data left
    {
      if ((HEAP_CHUNK_DATA_SIZE - node->memory_allocated) > length) //check if we can fit it
      {
        bool success = false;
        uint32_t return_value = heap_allocate_from_node(node, length, &success);
        if (success == true)
          return (uint32_t*)return_value;
      }      
    }
    node = node->next;
  }
  
  //if we got to here, then no free chunks were found of the correct size. IN order to fix this, 
  //add another node to the list, and select it. Then call heap_allocate_from_node()
  
  return NULL;
}

uint32_t heap_allocate_from_node(heap_node_t* node, uint32_t length, bool* success)
{
  for (uint32_t bit = 0; bit < HEAP_CHUNK_DATA_SIZE-1; bit++)//loop through each bit in the bitmap
  {
    if (test_bit(bit%8, &(node->heap_chunk_bitmap[bit/8])))     //if a free on is found, loop for count
    {
      bool completed_loop = true;
      for (uint32_t i = 0; i < length; i++)
      {
        if (!test_bit((bit+i)%8, &(node->heap_chunk_bitmap[(bit+i)/8])))
        {
          completed_loop = false;
          break;
        }
      }
      //if it got through all the bits succesfully, 'completed_loop' = true
      if (completed_loop)
      {
        mark_bits(&(node->heap_chunk_bitmap[bit/8]), length);
        
      }
      else//else, increase 'bit' by the amount we just traversed (becase we already checked it) 
      {
        
      }
    }
  }
  return 0;
}

uint32_t mark_bits(uint8_t* bitmap_address, uint32_t bit_count)
{
  return 0;
}

int heap_free(uint32_t* addr, uint32_t length)
{
  return 1;
}
