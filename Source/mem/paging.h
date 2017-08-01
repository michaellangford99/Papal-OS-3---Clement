#ifndef PAGING_H
#define PAGING_H

#define PAGE_SIZE                   4096

#define PAGE_DIR_ENTRY_RW           1
#define PAGE_DIR_ENTRY_READ_ONLY    0
#define PAGE_DIR_ENTRY_USER         1
#define PAGE_DIR_ENTRY_SUPERVISOR   0
#define PAGE_DIR_ENTRY_PRESENT      1
#define PAGE_DIR_ENTRY_NOT_PRESENT  0

typedef struct {
  uint32_t page_table[1024];
} __attribute__((packed)) page_table_t;

/*  partly sourced from James Molloy's tutorials: */
typedef struct {
  uint8_t present;    // Page present in memory
  uint8_t rw;         // Read-only if clear, readwrite if set
  uint8_t user;       // Supervisor level only if clear
  uint8_t accessed;   // Has the page been accessed since last refresh?
  uint8_t dirty;      // Has the page been written to since last refresh?
  uint8_t unused;     // unused and reserved bits
  uint32_t frame;     // Frame address (shifted right 12 bits)
} page_table_entry_descriptor_t;

int init_paging();
int get_physical_address(int page_table, int page);

uint32_t* get_physaddr(uint32_t * virtualaddr);
void map_page(uint32_t * physaddr, uint32_t * virtualaddr, unsigned int flags);

#endif