#include <system.h>

uint32_t chunks_allocated = 0;

heap_node_t* heap_list;

int heap_init()
{
  printf("heap_init\n");
  printf("....size of heap node is %d bytes\n", sizeof(heap_node_t));
  printf("....data+bitmap+header: %d\n", HEAP_CHUNK_HEADER_SIZE+HEAP_CHUNK_BITMAP_SIZE+HEAP_CHUNK_DATA_SIZE);
  
  heap_list = (heap_node_t*)kmalloc(sizeof(heap_node_t));
  memset((char*)heap_list, 0, sizeof(heap_node_t));
  heap_list->next = NULL;

  printf("....heap_list address: 0x%x\n", (uint32_t)heap_list);

  return 1;
}

uint32_t* heap_alloc(uint32_t length)
{
  //first check to ensure that the amount of memory requested is able to be supplied. Ideally, if more than 3KB is needed,
  //kmalloc should be used
  //...

  //printf("heap_alloc: called with length: 0x%x\n", length);

  if (length > HEAP_CHUNK_DATA_SIZE)
    return kmalloc(length);
  
  heap_node_t* node = heap_list;
  
  bool found_free_node = false;
  while(true)//loop through nodes
  {
    //printf("....heap_alloc: node address: 0x%x\n", (uint32_t)node);
    //printf("....heap_alloc: memory_allocated: 0x%x\n", node->memory_allocated);
    if (node->memory_allocated < HEAP_CHUNK_DATA_SIZE) //check if there is any data left
    {
      //printf("....heap_alloc: there is 0x%x data left in this node\n", HEAP_CHUNK_DATA_SIZE - node->memory_allocated);
      if ((HEAP_CHUNK_DATA_SIZE - node->memory_allocated) > length) //check if we can fit it
      {
        //printf("....heap_alloc: this node has enough data by 0x%x bytes\n", (HEAP_CHUNK_DATA_SIZE - node->memory_allocated) - length);
        bool success = false;
        uint32_t return_value = heap_allocate_from_node(node, length, &success);
        if (success == true)
          return (uint32_t*)return_value;
        //printf("....heap_alloc: unsuccessful node\n");
      }      
    }
    if (node->next != NULL)
      node = node->next;
    else
      break;
  }

  //if we got to here, then no free chunks were found of the correct size. In order to fix this,
  //add another node to the list, and select it. Then call heap_allocate_from_node()

  //printf("....heap_alloc: previous node did not contain 0x%x bytes of free memory; allocating new node\n", length);
  //TODO: erase the block first
  node->next = (heap_node_t*)kmalloc(sizeof(heap_node_t));
  node = node->next;
  memset((char*)(node), 0, sizeof(heap_node_t));
  
  //printf("....heap_alloc: new node address: 0x%x\n", (uint32_t)node);

  bool success;
  uint32_t return_value = heap_allocate_from_node(node, length, &success);
  if (success == true)
    return (uint32_t *)return_value;

  return NULL;
}

uint32_t heap_allocate_from_node(heap_node_t* node, uint32_t length, bool* success)
{
  //printf("........heap_allocate_from_node: node passed: 0x%x\n", (uint32_t)node);
  //printf("........heap_allocate_from_node: length passed: 0x%x\n", length);
  //convert length to uint32s
  length = length/4 + (length%4 != 0);
  
  //printf("........heap_allocate_from_node: length (in uint32_ts): 0x%x\n", length);

  for (uint32_t bit = 0; bit < HEAP_CHUNK_BITMAP_SIZE*8; bit++)//loop through each bit in the bitmap
  {
    if (!test_bit(bit%8, &(node->heap_chunk_bitmap[bit/8])))     //if a free on is found, loop for count
    {
      //printf("........heap_allocate_from_node: bit: 0x%x is free\n", bit);
      bool completed_loop = true;
      for (uint32_t i = 0; i < length; i++)
      {
        if (test_bit((bit+i)%8, &(node->heap_chunk_bitmap[(bit+i)/8])))
        {
          //printf("........heap_allocate_from_node: i: 0x%x, bit+i: 0x%x - is used\n", i, bit+i);
          completed_loop = false;
          //increase 'bit' by the amount we just traversed (becase we already checked it) 
          bit += i;
          break;
        }

        if ((bit+i) > HEAP_CHUNK_BITMAP_SIZE*8)
        {
          //printf("........heap_allocate_from_node: reached end of node\n");
          completed_loop = false;
          bit += i;
          break;
        }
      }
      //if it got through all the bits succesfully, 'completed_loop' = true
      if (completed_loop)
      {
        //printf("........heap_allocate_from_node: completed_loop=true\n");
        mark_bits(node, bit, length);
        *success = true;
        node->memory_allocated += length*4;      
        return (uint32_t)&(node->heap_data[bit]);   
      }
    }
  }
  return 0;
}

void mark_bits(heap_node_t* node, uint32_t bit, uint32_t length)
{
  for (uint32_t i = 0; i < length; i++)
  {
    set_bit((bit+i)%8, &(node->heap_chunk_bitmap[(bit+i)/8]));
  }
}

int heap_free(uint32_t* addr, uint32_t length)
{
  return 1;
}
