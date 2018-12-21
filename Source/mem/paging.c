#include <system.h>

page_directory_entry_t* page_directory_entries;//[1024] __attribute__((aligned(4096)));
page_table_t* page_tables;//[1024] __attribute__((aligned(4096)));

//link to assembly routines
extern void enablePaging();
extern void invalidate_page(uint32_t*);

int init_paging() {
  printf("init_paging\n");
  //allocate 4KB for page directory

  page_directory_entries = (page_directory_entry_t*)kmalloc(1024*4);
  page_tables = (page_table_t*)kmalloc(1024 * sizeof(page_table_t));

  printf("....page_directory_entries addr: 0x%x / %d / %d KB\n", (uint32_t)&page_directory_entries[0], (uint32_t)&page_directory_entries[0], ((uint32_t)&page_directory_entries[0])/1024);

  //first, set all entries in page directory to NP
  unsigned int i;
  for(i = 0; i < 1024; i++)
  {
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_directory_entries[i].value = 0;
      page_directory_entries[i].page_dir_entry_bits.user = PDE_PTE_SUPERVISOR;
      page_directory_entries[i].page_dir_entry_bits.present = PDE_PTE_NOT_PRESENT;
      page_directory_entries[i].page_dir_entry_bits.rw = PDE_PTE_RW;

  }

  //Fill all 1024 entries in all 1024 page tables
  for (int pt = 0; pt < 1024; pt++)
  {
    for(i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        page_tables[pt].pages[i].value = ((1024*pt + i) * 0x1000);
        page_tables[pt].pages[i].page_table_entry_bits.user = PDE_PTE_SUPERVISOR;
        page_tables[pt].pages[i].page_table_entry_bits.present = PDE_PTE_PRESENT;
        page_tables[pt].pages[i].page_table_entry_bits.rw = PDE_PTE_RW;

    }
  }

  uint32_t kernel_size = get_kernel_size();

  uint32_t kernel_pages = kernel_size / PAGE_SIZE;
  if ((kernel_size % PAGE_SIZE) > 0)
    kernel_pages++;

  uint32_t kernel_page_tables = kernel_pages / 1024;
  if ((kernel_pages % PAGE_SIZE) > 0)
    kernel_page_tables++;

  uint32_t kernel_first_page_table = (get_kernel_location() / 4096)/1024;
  uint32_t kernel_first_page = (get_kernel_location() / 4096) % 1024;

  printf("....pages of memory in kernel: %d\n", kernel_pages);
  printf("....page tables of memory containing kernel: %d\n", kernel_page_tables);
  printf("....kernel_first_page_table: %d\n", kernel_first_page_table);
  printf("....kernel_first_page: %d\n", kernel_first_page);

  //put all 1024 page tables into the page directory
  // attributes: supervisor level, read/write, present
  for (i = 0; i < 1023; i++)
  {
    page_directory_entries[i].value = ((unsigned int)&page_tables[i]) & 0xFFFFF000;
    page_directory_entries[i].page_dir_entry_bits.user = PDE_PTE_SUPERVISOR;
    page_directory_entries[i].page_dir_entry_bits.present = PDE_PTE_PRESENT;
    page_directory_entries[i].page_dir_entry_bits.rw = PDE_PTE_RW;
  }

  //map last page directory to itself
  page_directory_entries[1023].value = ((unsigned int)&page_directory_entries[0]) & 0xFFFFF000;
  page_directory_entries[1023].page_dir_entry_bits.user = PDE_PTE_SUPERVISOR;
  page_directory_entries[1023].page_dir_entry_bits.present = PDE_PTE_PRESENT;
  page_directory_entries[1023].page_dir_entry_bits.rw = PDE_PTE_RW;

  loadPageDirectory((uint32_t*)&(page_directory_entries[0]));
  enablePaging();

  printf("....physical address that %x is mapped to: %x\n", 0xFFFFF000, (uint32_t)get_physaddr((uint32_t*)0xFFFFF000));

  return K_SUCCESS;
}

uint32_t * get_physaddr(uint32_t * virtualaddr) {
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;
    page_table_entry_t * pt = ((page_table_entry_t *)0xFFC00000) + (0x400 * pdindex);

    //check that both the page table / page directory entry,
    //and the page / page table entry are present.

    if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
      return NULL;
    if (pt[ptindex].page_table_entry_bits.present != PDE_PTE_PRESENT)
      return NULL;

    return (uint32_t *)((pt[ptindex].value & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}
void map_page(uint32_t * physaddr, uint32_t * virtualaddr, uint8_t privilege_level, uint8_t rw, uint8_t force) {
    // Make sure that both addresses are page-aligned.

    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;

    // check whether the PD entry is present.
    // When it is not present, create a new empty PT and
    // adjust the PDE accordingly.
    if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
    {
      page_table_t* new_page_table = (page_table_t*)kmalloc(sizeof(page_table_t));

      pd[pdindex].value = ((unsigned int)new_page_table) & 0xFFFFF000;

      pd[pdindex].page_dir_entry_bits.user = (privilege_level == DPL_0) ? PDE_PTE_SUPERVISOR : PDE_PTE_USER;
      pd[pdindex].page_dir_entry_bits.present = PDE_PTE_PRESENT;
      pd[pdindex].page_dir_entry_bits.rw = rw;
    }

    page_table_entry_t * pt = ((page_table_entry_t *)0xFFC00000) + (0x400 * pdindex);

    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present.
    // if force is off, and the page is already mapped, return
    if ((pt[ptindex].page_table_entry_bits.present == PDE_PTE_PRESENT) && (force == 0))
    {
      printf("attempted to map virtual address 0x%x to 0x%x, which is already mapped to 0x%x!\n", (uint32_t)virtualaddr, (uint32_t)physaddr, (uint32_t)get_physaddr(virtualaddr));
      return;
    }

    pt[ptindex].value = ((uint32_t)physaddr) & 0xFFFFF000;
    pt[ptindex].page_table_entry_bits.user = (privilege_level == DPL_0) ? PDE_PTE_SUPERVISOR : PDE_PTE_USER;
    pt[ptindex].page_table_entry_bits.present = PDE_PTE_PRESENT;
    pt[ptindex].page_table_entry_bits.rw = rw;

    invalidate_page(physaddr);
}
void set_memory_range_dpl(uint32_t virt_address, uint32_t length, uint8_t privilege_level) {
    uint32_t aligned_address = virt_address & 0xFFFFF000;

    length += (virt_address - aligned_address);

    uint32_t page_count = (length/PAGE_SIZE) + ((length%PAGE_SIZE) != 0);

    for (uint32_t i = 0; i < page_count; i++)
    {
      set_page_dpl(aligned_address + i*PAGE_SIZE, privilege_level);
      //printf("paging.c - set_memory_range_dpl: virt_address=0x%x, page address=0x%x, privilege_level=0x%x\n", aligned_address, aligned_address+i*PAGE_SIZE, privilege_level);
    }
}
void set_page_dpl(uint32_t virt_address, uint8_t privilege_level) {
  virt_address = virt_address & 0xFFFFF000;
  uint32_t pdindex = (uint32_t)virt_address >> 22;
  uint32_t ptindex = (uint32_t)virt_address >> 12 & 0x03FF;

  page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;
  page_table_entry_t * pt = ((page_table_entry_t *)0xFFC00000) + (0x400 * pdindex);

  //check that both the page table / page directory entry,
  //and the page / page table entry are present.

  if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
    return;
  if (pt[ptindex].page_table_entry_bits.present != PDE_PTE_PRESENT)
    return;

  pt[ptindex].page_table_entry_bits.user = (privilege_level == DPL_0) ? PDE_PTE_SUPERVISOR : PDE_PTE_USER;
  //only set the page directory's dpl if it requires higher privilege than the page in it
  if ((privilege_level != DPL_0) && (pd[pdindex].page_dir_entry_bits.user == PDE_PTE_SUPERVISOR))
    pd[pdindex].page_dir_entry_bits.user = PDE_PTE_USER;

  invalidate_page((uint32_t*)(page_tables[pdindex].pages[ptindex].value & 0xFFFFF000));
}
void set_page_table_dpl(uint32_t virt_address, uint8_t privilege_level) {
  virt_address = virt_address & 0xFFFFF000;
  uint32_t pdindex = (uint32_t)virt_address >> 22;
  uint32_t ptindex = (uint32_t)virt_address >> 12 & 0x03FF;

  page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;

  //if page dir isn't present, return
  if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
    return;

  //write privilege level
  pd[pdindex].page_dir_entry_bits.user = (privilege_level == DPL_0) ? PDE_PTE_SUPERVISOR : PDE_PTE_USER;//.value = (page_directory_entries[pdindex].value & (0xFFFFFFFB)) | ((privilege_level == DPL_3) << 2);
}
void set_page_table_present(uint32_t virt_address, uint8_t present) {
  virt_address = virt_address & 0xFFFFF000;
  uint32_t pdindex = (uint32_t)virt_address >> 22;
  uint32_t ptindex = (uint32_t)virt_address >> 12 & 0x03FF;

  page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;

  //if page dir isn't present, return
  if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
    return;

  pd[pdindex].page_dir_entry_bits.present = present;
}
void set_page_present(uint32_t virt_address, uint8_t present) {
  virt_address = virt_address & 0xFFFFF000;
  uint32_t pdindex = (uint32_t)virt_address >> 22;
  uint32_t ptindex = (uint32_t)virt_address >> 12 & 0x03FF;

  page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;
  page_table_entry_t * pt = ((page_table_entry_t *)0xFFC00000) + (0x400 * pdindex);

  //check that both the page table / page directory entry,
  //and the page / page table entry are present.

  if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
    return;
  if (pt[ptindex].page_table_entry_bits.present != PDE_PTE_PRESENT)
    return;

  pt[ptindex].page_table_entry_bits.present = present;
}
void unmap_page(uint32_t virt_address) {
  //bounds check - let's make sure we're not unmapping something we'll regret later..
  uint32_t kernel_start = get_kernel_location() & 0xFFFFF000;
  uint32_t kernel_end = get_kernel_end();
  if (kernel_end%0x1000 != 0)
    kernel_end += 0x1000-(kernel_end%0x1000);
  kernel_end &= 0xFFFFF000;

  if ((virt_address >= kernel_start) && (virt_address <= kernel_end))
  {
    printf("attempted to unmap kernel-space page: virtual address  0x%x!\n", (uint32_t)virt_address);
    return;
  }

  set_page_present(virt_address, PDE_PTE_NOT_PRESENT);
}
void unmap_page_table(uint32_t virt_address) {
  //bounds check - let's make sure we're not unmapping something we'll regret later..
  uint32_t kernel_start = get_kernel_location() & 0xFFFFF000;
  uint32_t kernel_end = get_kernel_end();
  if (kernel_end%0x1000 != 0)
    kernel_end += 0x1000-(kernel_end%0x1000);
  kernel_end &= 0xFFFFF000;

  if ((virt_address >= kernel_start) && (virt_address <= kernel_end))
  {
    printf("attempted to unmap kernel-space page table: virtual address 0x%x!\n", (uint32_t)virt_address);
    return;
  }

  set_page_table_present(virt_address, PDE_PTE_NOT_PRESENT);
}
uint32_t swap_page_table(uint32_t virt_address, uint32_t page_table_phys_address, uint8_t privilege_level, uint8_t rw) {
  //bounds check - let's make sure we're not swapping something we'll regret later..
  uint32_t kernel_start = get_kernel_location() & 0xFFFFF000;
  uint32_t kernel_end = get_kernel_end();
  if (kernel_end%0x1000 != 0)
    kernel_end += 0x1000-(kernel_end%0x1000);
  kernel_end &= 0xFFFFF000;

  virt_address = virt_address & 0xFFFFF000;
  uint32_t pdindex = (uint32_t)virt_address >> 22;
  uint32_t ptindex = (uint32_t)virt_address >> 12 & 0x03FF;

  page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;

  if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
  {
    printf("attempted to swap out page table that is not present yet: virtual address 0x%x!\n", virt_address);
    return 0;
  }

  if ((((pd[pdindex].value & 0xFFFFF000) >= kernel_start) && ((pd[pdindex].value & 0xFFFFF000) <= kernel_end)) ||
      ((virt_address >= kernel_start) && (virt_address <= kernel_end)))
  {
    printf("attempted to swap out kernel-space page table: physical address 0x%x, virtual address 0x%x!\n", (uint32_t)(pd[pdindex].value & 0xFFFFF000), virt_address);
    return 0;
  }

  uint32_t original_page_table = pd[pdindex].value;

  pd[pdindex].value = page_table_phys_address & 0xFFFFF000;
  pd[pdindex].page_dir_entry_bits.present = PDE_PTE_PRESENT;
  pd[pdindex].page_dir_entry_bits.user = (privilege_level != DPL_0) ? PDE_PTE_USER : PDE_PTE_SUPERVISOR;
  pd[pdindex].page_dir_entry_bits.rw = rw;

  return original_page_table;
}
uint32_t swap_page(uint32_t virt_address, uint32_t page_phys_address, uint8_t privilege_level, uint8_t rw) {
  //bounds check - let's make sure we're not swapping something we'll regret later..
  uint32_t kernel_start = get_kernel_location() & 0xFFFFF000;
  uint32_t kernel_end = get_kernel_end();
  if (kernel_end%0x1000 != 0)
    kernel_end += 0x1000-(kernel_end%0x1000);
  kernel_end &= 0xFFFFF000;

  virt_address = virt_address & 0xFFFFF000;
  uint32_t pdindex = (uint32_t)virt_address >> 22;
  uint32_t ptindex = (uint32_t)virt_address >> 12 & 0x03FF;

  page_directory_entry_t * pd = (page_directory_entry_t *)0xFFFFF000;
  page_table_entry_t * pt = ((page_table_entry_t *)0xFFC00000) + (0x400 * pdindex);

  //check that the pde is present. It's 'oh well' if the pte is not present

  if (pd[pdindex].page_dir_entry_bits.present != PDE_PTE_PRESENT)
  {
    printf("attempted to swap out page lacking a page table: virtual address 0x%x!\n", virt_address);
    return 0;
  }

  if (pt[ptindex].page_table_entry_bits.present != PDE_PTE_PRESENT)
  {
    printf("attempted to swap out page that is not present yet: virtual address 0x%x!\n", virt_address);
    return 0;
  }

  if ((((pt[ptindex].value & 0xFFFFF000) >= kernel_start) && ((pt[ptindex].value & 0xFFFFF000) <= kernel_end)) ||
      ((virt_address >= kernel_start) && (virt_address <= kernel_end)))
  {
    printf("attempted to swap out kernel-space page: physical address 0x%x, virtual address 0x%x!\n", (uint32_t)(pt[ptindex].value & 0xFFFFF000), virt_address);
    return 0;
  }

  uint32_t original_page = pt[ptindex].value;

  pt[ptindex].value = page_phys_address & 0xFFFFF000;
  pt[ptindex].page_table_entry_bits.present = PDE_PTE_PRESENT;
  pt[ptindex].page_table_entry_bits.user = (privilege_level != DPL_0) ? PDE_PTE_USER : PDE_PTE_SUPERVISOR;
  pt[ptindex].page_table_entry_bits.rw = rw;

  return original_page;
}
