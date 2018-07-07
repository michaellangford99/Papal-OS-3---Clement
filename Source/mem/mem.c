#include <system.h>

extern uint32_t __start;
extern uint32_t __end;

uint8_t* memmap_address;
uint32_t memory_slots;

uint32_t kernel_location;
uint32_t kernel_size;
uint32_t kernel_end;

uint32_t mem_map_start;
uint32_t mem_map_size;
uint32_t mem_map_end;

uint32_t total_memory_size = 0;
uint32_t first_free_block = 0;
uint8_t memory_bitmap[MEM_MNGR_SIZE_OF_MEMORY_BITMAP];

void mark_block(uint8_t value, uint64_t bit);
uint8_t test_block(uint64_t bit);

int memory_init(struct multiboot_header* mboot_header)
{

  //Step 1 - clean up memory slots - remove kernel from list of free memory, align on 4K boundaries, remove memory map from list of free memory...
  printf("memory_init:\n");
  memory_slots = mboot_header->mmap_length / sizeof(mboot_memmap_t);

  kernel_location = (uint32_t)&__start;
  kernel_size = (((uint32_t)&__end) - ((uint32_t)&__start));
  kernel_end = (uint32_t)&__end;

  mem_map_start = (uint32_t)mboot_header->mmap_addr;
  mem_map_size  = (uint32_t)mboot_header->mmap_length;
  mem_map_end = (uint32_t)mboot_header->mmap_addr + (uint32_t)mboot_header->mmap_length;

  printf("....kernel location %d / %dKB\n", kernel_location, kernel_location/1024);
  printf("...........size     %d / %dKB\n", kernel_size, kernel_size/1024);
  printf("...........end      %d / %dKB\n", kernel_end, kernel_end/1024);

  mboot_memmap_t mmap_entries[memory_slots];
  memcpy((char*)&mmap_entries, (char*)mboot_header->mmap_addr, mboot_header->mmap_length);

  //exclude mods

  for (uint32_t mod = 0; mod < mboot_header->mods_count; mod++)
  {
    mboot_mod_desc_t* mod_info = (mboot_mod_desc_t*)(mboot_header->mods_addr + (mod * sizeof(mboot_mod_desc_t)));
    if (mod_info->end > kernel_end)
    {
      kernel_end = mod_info->end;
      kernel_size = kernel_end - kernel_location;
      printf("....extended 'kernel_end' to %d on mod%d\n", mod_info->end, mod);
    }
  }

  printf("....kernel location     %d / %dKB\n", kernel_location, kernel_location/1024);
  printf("....kernel+modules size %d / %dKB\n", kernel_size, kernel_size/1024);
  printf("....kernel+modules end  %d / %dKB\n", kernel_end, kernel_end/1024);


  for (uint32_t i = 0; i < memory_slots; i++)
  {
    if ((uint32_t)mmap_entries[i].type == MULTIBOOT_MMAP_FREE_MEMORY)
    {
      if ((uint32_t)mmap_entries[i].base_addr >= kernel_location)
      {

        //exclude kernel
        /*if (kernel_location >= (uint32_t)mmap_entries[i].base_addr &&
            kernel_end <= ((uint32_t)mmap_entries[i].length+(uint32_t)mmap_entries[i].base_addr))
        {
          //wastes some memory here if the two overlap
          mmap_entries[i].base_addr += (uint64_t)kernel_size;
          mmap_entries[i].length -= (uint64_t)kernel_size;
          printf("....::::shrunk slot %d %d / %dKB\n", i, (uint32_t)kernel_size, (uint32_t)kernel_size / MEM_MNGR_SLOT_SIZE);
        }

        //exclude memory map
        if (mem_map_start >= (uint32_t)mmap_entries[i].base_addr &&
            mem_map_end <= ((uint32_t)mmap_entries[i].length+(uint32_t)mmap_entries[i].base_addr))
        {
          //same here
          mmap_entries[i].base_addr += (uint64_t)mem_map_size;
          mmap_entries[i].length -= (uint64_t)mem_map_size;
          printf("....::::shrunk slot %d %d / %dKB\n", i, (uint32_t)mem_map_size, (uint32_t)mem_map_size / MEM_MNGR_SLOT_SIZE);
        }*/

        //align slot on 4K boundary
        uint32_t decrease = MEM_MNGR_ALIGN - ((uint32_t)mmap_entries[i].base_addr % MEM_MNGR_ALIGN);
        decrease = decrease % MEM_MNGR_ALIGN;
        mmap_entries[i].base_addr += (uint64_t)decrease;
        mmap_entries[i].length -= (uint64_t)decrease;

        mmap_entries[i].length -= mmap_entries[i].length % (uint64_t)MEM_MNGR_SLOT_SIZE; // make each entry try to fit in block size


        total_memory_size +=  (uint32_t)mmap_entries[i].length;
      }
      else{
        mmap_entries[i].type = MULTIBOOT_MMAP_RESERVED;
        printf("....----forced memory slot %d into reserved type, as its base address is below KERNEL_START----\n", i);
      }
    }
  }

  for (uint32_t i = 0; i < memory_slots; i++)
  {
    printf("....Memory Map Entry - %d:\n", i);
    printf("....  base_addr: %x, %d KB\n", (uint32_t)mmap_entries[i].base_addr, (uint32_t)mmap_entries[i].base_addr/1024);
    printf("....  length   : %x, %d KB\n", (uint32_t)mmap_entries[i].length   , (uint32_t)mmap_entries[i].length/1024);
    printf("....  type     : %d\n", (uint32_t)mmap_entries[i].type);
    printf("\n");
  }

  printf("....total free memory: %d, %d KB, %d MB\n", total_memory_size, total_memory_size/1024, total_memory_size/(1024*1024));

  //Now we have to mark every slot of memory as used, then go throught every free slot, and mark it as free. This way we won't miss any.

  //mark all as used

  printf("....size of memory bitmap: %d\n", MEM_MNGR_SIZE_OF_MEMORY_BITMAP);
  memset((char*)(&memory_bitmap), 255, MEM_MNGR_SIZE_OF_MEMORY_BITMAP); //255 >> binary == %11111111 - all used


  //mark free as free
  for (uint32_t i = 0; i < memory_slots; i++)
  {
    if ((uint32_t)mmap_entries[i].type == MULTIBOOT_MMAP_FREE_MEMORY)
    {
      uint32_t starting_chunk = (uint32_t)(mmap_entries[i].base_addr / (4096));
      printf("....starting 4K page of memory slot %d is : %d\n", i, starting_chunk);
      uint32_t ending_chunk = starting_chunk + (uint32_t)(mmap_entries[i].length / (4096));
      printf("....ending 4K page of memory slot %d is : %d\n", i, ending_chunk);

      mark_block(0, starting_chunk);
      mark_block(0, ending_chunk);
      for (uint32_t _4kblock = starting_chunk; _4kblock < ending_chunk+1; _4kblock++)
      {
        mark_block(0, _4kblock);
      }
    }
  }

  //forcibly mark areas of memory that can't be allocated as so.
  uint32_t kernel_blocks = kernel_size/4096 + (kernel_size%4096 != 0);
  for (uint32_t i = 0; i < kernel_blocks; i++)
  {
    mark_block(1, kernel_location/4096 + i);
  }

  uint32_t mem_map_size_aligned = mem_map_size + (4096-mem_map_size%4096) + mem_map_start%4096;
  uint32_t mem_map_start_aligned = mem_map_start - mem_map_start%4096;

  uint32_t memory_map_blocks = mem_map_size_aligned/4096 + (mem_map_size_aligned%4096 != 0);

  for (uint32_t i = 0; i < memory_map_blocks; i++)
  {
    mark_block(1, (uint32_t)mem_map_start_aligned/4096 + i);
  }

  //now we nead to copy back the new memory map table:
  memcpy((char*)mboot_header->mmap_addr, (char*)&mmap_entries, mboot_header->mmap_length);

  return K_SUCCESS;
}

uint32_t* kmalloc(uint32_t length)
{
  uint32_t mem_blocks = (length / MEM_MNGR_SLOT_SIZE);  //number of blocks needed to grant requested memory
  if (length % MEM_MNGR_SLOT_SIZE != 0)            // if it isn't a multiple of 4K, add a block to the amount of needed blocks
    mem_blocks += 1;
  //printf("allocating %d bytes....\n", length)  ;
  //printf("allocating %d blocks...\n", mem_blocks);

  mboot_memmap_t mmap_entries[memory_slots];

  struct multiboot_header* mboot_header;
  mboot_header = multiboot_get_address();
  memcpy((char*)&mmap_entries, (char*)mboot_header->mmap_addr, mboot_header->mmap_length);

  //go through memory bitmap, searching for a free block:

  uint32_t free_chunk_start = 0;
  uint32_t free_chunk_end = 0;

  for (uint32_t i = 0; i < MEM_MNGR_SIZE_OF_MEMORY_BITMAP; i++)
  {
    if (test_block(i) == MEM_MNGR_FREE_SLOT)
    {
      //printf("found free block corresponding to address %d\n", i * 4096);
      //if we find a free block, keep going until we find the proper number...
      for (uint32_t j = 0; j < mem_blocks; j++)
      {

        if (test_block(i + j) != MEM_MNGR_FREE_SLOT)
        {
          i += j;
          break;
        }
        //did we get through?
        if (j == mem_blocks-1)
        {

          free_chunk_start = i;
          free_chunk_end = i+j;
          for (uint32_t allocated_blocks = free_chunk_start; allocated_blocks < (free_chunk_end+1); allocated_blocks++)
          {
            mark_block(MEM_MNGR_USED_SLOT, allocated_blocks);
          }
          //printf("%d\n", free_chunk_start * 4096);

          //first_free_block = free_chunk_end;
          return (uint32_t*)(free_chunk_start * 4096);
        }
      }
    }
  }
  printf("!!!!!MEMORY MANAGER WAS UNABLE TO FIND FREE MEMORY OF SIZE %d bytes !!!!!!!\n", length);
  return (uint32_t*)0;
}

uint32_t kfree(uint32_t* base, uint32_t length)
{
  uint32_t mem_blocks = (length / MEM_MNGR_SLOT_SIZE);  //number of blocks needed to release requested memory
  if (length % MEM_MNGR_SLOT_SIZE != 0)            // if it isn't a multiple of 1K, add a block to the nedded blocks
    mem_blocks += 1;

  if ((uint32_t)base % 4096 != 0) { return K_ERROR_M; }
  //first_free_block = ((uint32_t)base / 4096);
  for (uint32_t i = 0; i < (length/4096); i++)
  {
    mark_block(MEM_MNGR_FREE_SLOT, ((uint32_t)base / 4096) + i);
  }

  return K_SUCCESS;
}

uint32_t kunfree(uint32_t* base, uint32_t length)
{
  uint32_t mem_blocks = (length / MEM_MNGR_SLOT_SIZE);  //number of blocks needed to release requested memory
  if (length % MEM_MNGR_SLOT_SIZE != 0)            // if it isn't a multiple of 1K, add a block to the nedded blocks
    mem_blocks += 1;

  if ((uint32_t)base % 4096 != 0) { return K_ERROR_M; }

  for (uint32_t i = 0; i < (length/4096); i++)
  {
    mark_block(MEM_MNGR_USED_SLOT, ((uint32_t)base / 4096) + i);
  }

  return K_SUCCESS;
}

void mark_block(uint8_t value, uint64_t bit)
{
  uint32_t byte = (uint32_t)(bit / (uint64_t)8);
  uint32_t bit_in_byte = (uint32_t)(bit % (uint64_t)8);

  //printf("bit: %d\n", (uint32_t)bit);
  //printf("byte: %d\n", (uint32_t)byte);
  //printf("bit_in_byte: %d\n", (uint32_t)bit_in_byte);
  //printf("address of bitmap: %d\n", &memory_bitmap[0]);

  if (value == 1)
  {
    set_bit(bit_in_byte, (uint8_t*)(&memory_bitmap[0] + byte));
  }
  if (value == 0)
  {
    clear_bit(bit_in_byte, (uint8_t*)(&memory_bitmap[0] + byte));
  }

}

uint8_t test_block(uint64_t bit)
{
  uint32_t byte = (uint32_t)(bit / (uint64_t)8);
  uint32_t bit_in_byte = (uint32_t)(bit % (uint64_t)8);

  //printf("bit: %d\n", (uint32_t)bit);
  //printf("byte: %d\n", (uint32_t)byte);
  //printf("bit_in_byte: %d\n", (uint32_t)bit_in_byte);
  //printf("address of bitmap: %d\n", &memory_bitmap[0]);
  uint8_t result = (uint8_t)test_bit(bit_in_byte, (uint8_t*)(&memory_bitmap[0] + byte));
  //printf(" %d ", (uint32_t)result);
  return result;

}

uint32_t get_kernel_location() { return kernel_location; }
uint32_t get_kernel_size() { return kernel_size; }
uint32_t get_kernel_end() { return kernel_end; }

memory_info_t get_memory_info() {
  memory_info_t m;
  m.kernel_location=kernel_location;
  m.kernel_size=kernel_size;
  m.kernel_end=kernel_end;
  m.mem_map_start=mem_map_start;
  m.mem_map_size=mem_map_size;
  m.mem_map_end=mem_map_end;
  m.total_memory_size=total_memory_size;

  return m;
}
