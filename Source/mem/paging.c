#include <system.h>

uint32_t* page_directory;//[1024] __attribute__((aligned(4096)));
page_table_t* page_tables;//[1024] __attribute__((aligned(4096)));

//link to assembly routines
extern void enablePaging();
extern void invalidate_page(uint32_t*);

int init_paging() {
  /*
  This basic paging code just identity maps the entire 4GB address space.
  */
  printf("init_paging\n");
  //allocate 4KB for page directory
  page_directory = kmalloc(1024*4);

  memset((char*)&page_directory[0], 0, 1024 * 4);
  page_tables = (page_table_t*)kmalloc(1024 * sizeof(page_table_t));

  printf("....page_directory addr: 0x%x / %d / %d KB\n", (uint32_t)&page_directory[0], (uint32_t)&page_directory[0], ((uint32_t)&page_directory[0])/1024);

  //first, set all entries in page directory to NP
  unsigned int i;
  for(i = 0; i < 1024; i++)
  {
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_directory[i] = 0b000000110;
  }

  //Fill all 1024 entries in all 1024 page tables
  for (int pt = 0; pt < 1024; pt++)
  {
    for(i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        page_tables[pt].page_table[i] = ((1024*pt + i) * 0x1000) | 0b1111; // attributes: user level, read/write, present.
    }
  }

  uint32_t kernel_size = get_kernel_size();

  uint32_t kernel_pages = kernel_size / PAGE_SIZE;
  if ((kernel_size % PAGE_SIZE) > 0)
    kernel_pages++;

  uint32_t kernel_page_tables = kernel_pages / 1024;
  if ((kernel_pages % PAGE_SIZE) > 0)
    kernel_page_tables++;

  printf("....pages of memory in kernel: %d\n", kernel_pages);
  printf("....page tables of memory containing kernel: %d\n", kernel_page_tables);

  uint32_t kernel_first_page_table = (get_kernel_location() / 4096)/1024;
  uint32_t kernel_first_page = (get_kernel_location() / 4096) % 1024;

  printf("....kernel_first_page_table: %d\n", kernel_first_page_table);
  printf("....kernel_first_page: %d\n", kernel_first_page);

  //put all 1024 page tables into the page directory
  // attributes: supervisor level, read/write, present
  for (i = 0; i < 1023; i++)
  {
    page_directory[i] = ((unsigned int)&page_tables[i]) | 0b111;
  }

  //map last page directory to itself
  page_directory[1023] = ((unsigned int)&page_directory[0]) | 0b111;

  loadPageDirectory(&(page_directory[0]));
  enablePaging();

  printf("....XXXXXXXXXXXX-TEST-XXXXXXXXX\n");

  printf("....physical address that %x is mapped to: %x\n", 0xFFFFF000, (uint32_t)get_physaddr((uint32_t*)0xFFFFF000));

  return K_SUCCESS;
}

/*uint32_t encode_page_directory_entry(uint32_t* page_table_address,
                                uint8_t page_size,
                                uint8_t write_through,
                                uint8_t privelege,
                                uint8_t rw,
                                uint8_t present)
{
  uint32_t page_dir_entry = (uint32_t)page_table_address;

  //set present bit
  if (present == PAGE_DIR_ENTRY_PRESENT)
    set_bit(0, (uint8_t*)&page_dir_entry);
  if (present == PAGE_DIR_ENTRY_NOT_PRESENT)
    clear_bit(0, (uint8_t*)&page_dir_entry);

  //set rw flag
  if (rw == PAGE_DIR_ENTRY_RW)
    set_bit(1, (uint8_t*)&page_dir_entry);
  if (rw == PAGE_DIR_ENTRY_READ_ONLY)
    clear_bit(1, (uint8_t*)&page_dir_entry);

  //set the supervisor bit
  if (privelege == PAGE_DIR_ENTRY_SUPERVISOR)
    clear_bit(2, (uint8_t*)&page_dir_entry);
  if (privelege == PAGE_DIR_ENTRY_USER)
    set_bit(2, (uint8_t*)&page_dir_entry);

  return 0;
}*/


uint32_t * get_physaddr(uint32_t * virtualaddr)
{
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    uint32_t * pd = (uint32_t *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.

    uint32_t * pt = ((uint32_t *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.

    return (uint32_t *)((pt[ptindex] & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}

void map_page(uint32_t * physaddr, uint32_t * virtualaddr, unsigned int flags)
{
    // Make sure that both addresses are page-aligned.

    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    uint32_t * pd = (uint32_t *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.

    uint32_t * pt = ((uint32_t *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?

    pt[ptindex] = ((uint32_t)physaddr) | (flags & 0xFFF) | 0x01; // Present

    invalidate_page(physaddr);
}
