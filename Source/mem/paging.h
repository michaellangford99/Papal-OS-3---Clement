#ifndef PAGING_H
#define PAGING_H

#define PAGE_SIZE                   4096

#define PDE_PTE_RW           1
#define PDE_PTE_READ_ONLY    0
#define PDE_PTE_USER         1
#define PDE_PTE_SUPERVISOR   0
#define PDE_PTE_PRESENT      1
#define PDE_PTE_NOT_PRESENT  0

typedef union
{
  struct {
    uint8_t present : 1;              // Page table present in memory
    uint8_t rw : 1;                   // Read-only if clear, readwrite if set
    uint8_t user : 1;                 // Supervisor level only if clear
    uint8_t write_through : 1;        // Write-through capability enabled if set
    uint8_t cache_dis : 1;            // Cache disabled if set
    uint8_t accessed : 1;             // Accessed if set
    uint8_t zero : 1;                 // always zero
    uint8_t page_size : 1;            // 0 for 4KB pages, 1 for 4 MB pages (requires PSE enabled)
    uint8_t g : 1;                    // ignored
    uint8_t unused : 3;               // unused and reserved bits
    uint32_t page_table_address : 20; // Page table address (shifted right 12 bits)
  } page_dir_entry_bits;
  uint32_t value;
} __attribute__((packed)) page_directory_entry_t;

typedef union
{
  struct {
    uint8_t present : 1;             // Page table present in memory
    uint8_t rw : 1;                  // Read-only if clear, readwrite if set
    uint8_t user : 1;                // Supervisor level only if clear
    uint8_t write_through : 1;       // Write-through capability enabled if set
    uint8_t cache_dis : 1;           // Cache disabled if set
    uint8_t accessed : 1;            // Accessed if set
    uint8_t dirty : 1;               // Has the page been written to since last refresh?
    uint8_t zero : 1;                // always zero
    uint8_t global : 1;              // if set, pervents TLB from updating cache address upon CR3 reset
    uint8_t unused : 3;              // unused and reserved bits
    uint32_t page_phys_address : 20; // physical address of page (shifted right 12 bits)
  } page_table_entry_bits;
  uint32_t value;
} __attribute__((packed)) page_table_entry_t;

typedef struct {
  page_table_entry_t pages[1024];
} __attribute__((packed)) page_table_t;

extern void loadPageDirectory(uint32_t*);//this one is in the header because other
                                         //files, aka pm.c will need to change the
                                         //page directory for each process

int init_paging();
uint32_t* get_physaddr(uint32_t * virtualaddr);
void map_page(uint32_t * physaddr, uint32_t * virtualaddr, uint8_t privilege_level, uint8_t rw, uint8_t force);
void set_memory_range_dpl(uint32_t virt_address, uint32_t length, uint8_t privilege_level);
void set_page_dpl(uint32_t virt_address, uint8_t privilege_level);
void set_page_table_dpl(uint32_t virt_address, uint8_t privilege_level);
void set_page_table_present(uint32_t virt_address, uint8_t present);
void set_page_present(uint32_t virt_address, uint8_t present);
void unmap_page(uint32_t virt_address);
void unmap_page_table(uint32_t virt_address);
uint32_t swap_page_table(uint32_t virt_address, uint32_t page_table_phys_address, uint8_t privilege_level, uint8_t rw);
uint32_t swap_page(uint32_t virt_address, uint32_t page_phys_address, uint8_t privilege_level, uint8_t rw);
#endif
