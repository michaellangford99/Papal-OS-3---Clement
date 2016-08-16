#include "../system.h"

uint32_t* page_directory;
//page_table_t* page_tables[1024]

uint32_t page_table_1[1024] __attribute__((aligned(4096)));

int init_paging() {
  page_directory = kmalloc(1024);
  
  //from : http://wiki.osdev.org/Setting_Up_Paging
  int i;
  for(i = 0; i < 1024; i++)
  {
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_directory[i] = 0x00000002;
  }
  
  // holds the physical address where we want to start mapping these pages to.
  // in this case, we want to map these pages to the very beginning of memory.
  unsigned int j;
   
  //we will fill all 1024 entries in the table, mapping 4 megabytes
  for(j = 0; j < 1024; j++)
  {
      // As the address is page aligned, it will always leave 12 bits zeroed.
      // Those bits are used by the attributes ;)
      page_table_1[j] = (j * 0x1000) | 3; // attributes: supervisor level, read/write, present.
  }
  
  // attributes: supervisor level, read/write, present
  page_directory[0] = ((unsigned int)first_page_table) | 3;
  
  return K_SUCCESS;
}