#include "../system.h"

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

uint64_t alloc_size = 0; 
uint64_t bitmap_size = 0;

uint32_t bitmap_addr = 0;

void mark_blocks_as_used(uint32_t start, uint32_t length);

//TODO align memory blocks on 1kb boundaries. will be useful once paging is enabled

int memory_init(struct multiboot_header* mboot_header)
{
  //1. find size & location of kernel
  //2. modify free slots to exclude memory, multiboot header, and kernel
  
  memory_slots = mboot_header->mmap_length / sizeof(mboot_memmap_t);
  
  kernel_location = (uint32_t)&__start;
  kernel_size = (((uint32_t)&__end) - ((uint32_t)&__start));
  kernel_end = (uint32_t)&__end;
  
  mem_map_start = (uint32_t)mboot_header->mmap_addr;
  mem_map_size  = (uint32_t)mboot_header->mmap_length;
  mem_map_end = (uint32_t)mboot_header->mmap_addr + (uint32_t)mboot_header->mmap_length;
  
  //printf("kernel location %d / %dKB\n", kernel_location, kernel_location/1024);
  //printf(".......size     %d / %dKB\n", kernel_size, kernel_size/1024);
  //printf(".......end      %d / %dKB\n", kernel_end, kernel_end/1024);  
  
  mboot_memmap_t mmap_entries[memory_slots];
  memcpy((char*)&mmap_entries, (char*)mboot_header->mmap_addr, mboot_header->mmap_length);
  
  for (uint32_t i = 0; i < memory_slots; i++)
  {
    if ((uint32_t)mmap_entries[i].type == MULTIBOOT_MMAP_FREE_MEMORY)
    {
      if ((uint32_t)mmap_entries[i].base_addr >= kernel_location)
      {
        
        //exclude kernel
        if (kernel_location >= (uint32_t)mmap_entries[i].base_addr && 
            kernel_end <= ((uint32_t)mmap_entries[i].length+(uint32_t)mmap_entries[i].base_addr))
        {
          mmap_entries[i].base_addr += (uint64_t)kernel_size;
          mmap_entries[i].length -= (uint64_t)kernel_size;
            //printf("::::shrunk slot %d %d / %dKB\n", i, (uint32_t)kernel_size, (uint32_t)kernel_size / MEM_MNGR_SLOT_SIZE);
        }
        
        //exclude memory map
        if (mem_map_start >= (uint32_t)mmap_entries[i].base_addr && 
            mem_map_end <= ((uint32_t)mmap_entries[i].length+(uint32_t)mmap_entries[i].base_addr))
        {
          mmap_entries[i].base_addr += (uint64_t)mem_map_size;
          mmap_entries[i].length -= (uint64_t)mem_map_size;
          //printf("::::shrunk slot %d %d / %dKB\n", i, (uint32_t)mem_map_size, (uint32_t)mem_map_size / MEM_MNGR_SLOT_SIZE);
        }
        
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
      }
    }
  }
  
  //printf("total free memory: %d\n", total_memory_size);
  
  //find size of bitmap
  
  alloc_size = ((uint64_t)(total_memory_size) * (uint64_t)MEM_MNGR_SLOT_SIZE) / (uint64_t)(MEM_MNGR_SLOT_SIZE+1);  
  bitmap_size = (uint64_t)(total_memory_size) - (uint64_t)(alloc_size);
  
  //printf("bitmap size: %d\n", (uint32_t)(bitmap_size));
  //printf("allocatable memory: %d\n", (uint32_t)(alloc_size));
   
  //find if bitmap will fit in a free slot
  
  for (uint32_t j = 0; j < memory_slots; j++)
  {
    if ((uint32_t)mmap_entries[j].type == MULTIBOOT_MMAP_FREE_MEMORY)
    {
      if ((uint32_t)mmap_entries[j].base_addr >= kernel_location)
      {
        if ((uint32_t)mmap_entries[j].length >= bitmap_size)
        {
          //if it does, save it there, and shrink the slot. allocate the bitmap for the memmanager
          bitmap_addr = (uint32_t)mmap_entries[j].base_addr;
          
          mmap_entries[j].base_addr += (uint64_t)bitmap_size;
          mmap_entries[j].length -= (uint64_t)bitmap_size;
          
          
          //align slot on 4K boundary
          uint32_t decrease = MEM_MNGR_ALIGN - ((uint32_t)mmap_entries[j].base_addr % MEM_MNGR_ALIGN);
          decrease = decrease % MEM_MNGR_ALIGN;
          mmap_entries[j].base_addr += (uint64_t)decrease;
          mmap_entries[j].length -= (uint64_t)decrease;
          
          mmap_entries[j].length -= mmap_entries[j].length % (uint64_t)MEM_MNGR_SLOT_SIZE; // make each entry try to fit in block size
          
          
          
          //printf("bitmap will fit in and has been stored in memory slot %d\n", j);
          break;
        }
      }
    }
  }
  
  //debug contents..
  /*for (uint32_t k = 0; k < memory_slots; k++)
  {
    printf("mmap entry     %d\n", k);
    printf("....entry size %d\n", (uint32_t)mmap_entries[k].size);
    printf("....addr       %d / %dKB\n", (uint32_t)mmap_entries[k].base_addr, (uint32_t)mmap_entries[k].base_addr/MEM_MNGR_SLOT_SIZE);
    printf("....length     %d / %dKB\n", (uint32_t)mmap_entries[k].length, (uint32_t)mmap_entries[k].length/MEM_MNGR_SLOT_SIZE);
    printf("....end addr   %d / %dKB\n", (uint32_t)mmap_entries[k].base_addr + (uint32_t)mmap_entries[k].length, ((uint32_t)mmap_entries[k].base_addr + (uint32_t)mmap_entries[k].length)/MEM_MNGR_SLOT_SIZE);
    printf("....type       %d\n\n", (uint32_t)mmap_entries[k].type);
  }*/
  
  //zero the bitmap, as all of it is unused.
  memset((char*)bitmap_addr, 0, bitmap_size);
  
  
  //now we nead to copy back the new memory map table:
  memcpy((char*)mboot_header->mmap_addr, (char*)&mmap_entries, mboot_header->mmap_length);
  
  return K_SUCCESS;
}

uint32_t* kmalloc(uint32_t length)
{
  uint32_t mem_blocks = (length / MEM_MNGR_SLOT_SIZE);  //number of blocks needed to grant requested memory
  if (length % MEM_MNGR_SLOT_SIZE != 0)            // if it isn't a multiple of 1K, add a block to the nedded blocks
    mem_blocks += 1;
  
  mboot_memmap_t mmap_entries[memory_slots];
  
  struct multiboot_header* mboot_header;
  mboot_header = multiboot_get_address();
  memcpy((char*)&mmap_entries, (char*)mboot_header->mmap_addr, mboot_header->mmap_length);
  
  //find a free block, then see if there are contiguous blocks for the length required
  //if so, mark them all as used, and return the pointer to the first block
  //if not, we need to compact the memory map! we can't do this until we have paging.
  uint32_t failed = 0;
  uint32_t slots_declined = 0;
  uint32_t* block_addr = (uint32_t*)(bitmap_addr);
  for (uint32_t i = 0; i < (uint32_t)bitmap_size; i++)
  {
    if (mem_blocks*MEM_MNGR_SLOT_SIZE > mmap_entries[slots_declined].length)
    {
      
      //printf("skipped slot %d, increased index by %d\n", slots_declined, (uint32_t)mmap_entries[slots_declined].length/MEM_MNGR_SLOT_SIZE + 1);
      i += (uint32_t)mmap_entries[slots_declined].length/MEM_MNGR_SLOT_SIZE + 1;
      slots_declined++;
    }
    //is the current block free?
    if (block_addr[i] == 0)
    {
      //printf("free block %d\n", i);
      //loop through...
      for (uint32_t cont = 0; cont < mem_blocks; cont++)
      {
        if (block_addr[i + cont] == 1)
        {
          goto no_contigous_slot;
        }
      }
      //a contiguous array of free blocks in the required number has been found. return the address and mark them as used.
      mark_blocks_as_used(i, mem_blocks);
      
      //find memory location pointed to by i
      
      uint32_t blocks_passed = 0;
      
      for (uint32_t slot = 0; slot < memory_slots; slot++)
      {
        if ((uint32_t)mmap_entries[slot].type == MULTIBOOT_MMAP_FREE_MEMORY)
        {
          if ((uint32_t)mmap_entries[slot].base_addr >= kernel_location)
          {
            blocks_passed += (uint32_t)mmap_entries[slot].length/MEM_MNGR_SLOT_SIZE;
            if (blocks_passed > i)
            {
              uint32_t addr_to_return = (i - (blocks_passed - (uint32_t)mmap_entries[slot].length/MEM_MNGR_SLOT_SIZE))*MEM_MNGR_SLOT_SIZE + (uint32_t)mmap_entries[slot].base_addr;
              //printf("returning address %d\n", addr_to_return);
              return (uint32_t*)addr_to_return;
            }
          }
        }
      }  
      
      //printf("a contiguous array of free blocks in the required number has been found.\n");
    }
    no_contigous_slot:
    failed++;
    
  }
  
  /*
  
  for (uint32_t i = 0; i < memory_slots; i++) {
    //printf("in memory slot %d, with base addr of %d\n", i, (uint32_t)mmap_entries[i].base_addr);
    if (mmap_entries[i].type == MULTIBOOT_MMAP_FREE_MEMORY) {
      //printf("..found free memory slot %d\n", i);
      
      for (uint32_t e = (uint32_t)mmap_entries[i].base_addr; e < (uint32_t)mmap_entries[i].base_addr + (uint32_t)mmap_entries[i].length; e += MEM_MNGR_SLOT_SIZE) {
        //printf("....in memory chunk %d\n", e);
        uint32_t* addr = (uint32_t*)e;
        
        if (addr[0] == MEM_MNGR_FREE_SLOT) {
          //printf("......found free chunk %d\n", e);
          for(uint32_t block = 0; block < mem_blocks; block++)
          {
            //printf("......looping through memory chunk %d\n", e+(block*MEM_MNGR_SLOT_SIZE));
            if (addr[block*MEM_MNGR_SLOT_SIZE] != MEM_MNGR_FREE_SLOT)
            {
              //printf("........chunk %d is not free\n", e+(block*MEM_MNGR_SLOT_SIZE));
              goto no_slot_found;
            }
            else
            {
              //printf("........chunk %d is free\n", e+(block*MEM_MNGR_SLOT_SIZE));
            }
          }
          for (uint32_t marking = 0; marking < mem_blocks; marking++)
          {
            uint32_t* marker = (uint32_t*)(e+(marking*MEM_MNGR_SLOT_SIZE));
            marker[0] = MEM_MNGR_USED_SLOT;
          }
          return (uint32_t*)(e);
          no_slot_found:
          failed++;
        }
        else
        {
          //printf("......chunk %d is not free\n", e);
        }
      }
    }
  }
  //printf("sorry, no memory slot was found with a length of %d blocks.\n", mem_blocks);
  
  return (uint32_t*)0;  
  //---------------------------------------------------------------------------------
  
  mboot_memmap_t mmap_entries[memory_slots];
  
  struct multiboot_header* mboot_header;
  mboot_header = multiboot_get_address();
  
  memcpy((char*)&mmap_entries, (char*)mboot_header->mmap_addr, mboot_header->mmap_length);
  
  int free_slots_found = 0;
  uint32_t addr_free_blocks[mem_blocks];
  
  for (uint32_t i = 0; i < memory_slots; i++)
  {
    if (mmap_entries[i].type == MULTIBOOT_MMAP_FREE_MEMORY)
    {
      for (uint32_t e = (uint32_t)mmap_entries[i].base_addr; e < (uint32_t)mmap_entries[i].base_addr + (uint32_t)mmap_entries[i].length; e += MEM_MNGR_SLOT_SIZE)
      {
        if ((uint64_t*)(e) == MEM_MNGR_FREE_SLOT)
        {
          free_slots_found++;
          if (free_slots_found > mem_blocks)
          {
            goto slot_search_done;
          }
          addr_free_blocks[free_slots_found-1] = e;
        }
      }
    }
  }
  
  slot_search_done:
  for (int i = 0; i < mem_blocks; i++)
  {
    uint64_t* ptr = (uint64_t*)addr_free_blocks[i];
    ptr[0] = MEM_MNGR_USED_SLOT;
    if (i  == (mem_blocks-1))
    {
      ptr[1] = MEM_MNGR_LAST_BLOCK;
    }
    else
    {
      ptr[1] = (uint64_t)addr_free_blocks[i+1];
    }
  }
  return (uint64_t*)addr_free_blocks[0];*/
  return (uint32_t*)0;
}

void mark_blocks_as_used(uint32_t start, uint32_t length)
{
  memset((char*)start, 1, length);
}


int kfree(uint32_t* base, uint32_t length)
{
  uint32_t mem_blocks = (length / MEM_MNGR_SLOT_SIZE);  //number of blocks needed to grant requested memory
  if (length % MEM_MNGR_SLOT_SIZE != 0)            // if it isn't a multiple of 1K, add a block to the nedded blocks
    mem_blocks += 1;
    
  memset((char*)base, 0, mem_blocks);
  return K_SUCCESS;
}

uint64_t get_bitmap_size() { return bitmap_size; }
uint32_t get_bitmap_addr() { return bitmap_addr; }

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
  m.alloc_size=alloc_size; 
  m.bitmap_size=bitmap_size;
  m.bitmap_addr=bitmap_addr;
  
  return m;
}
