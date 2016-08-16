#ifndef MEM_H
#define MEM_H

#define MEM_MNGR_1KB_SLOT         1
#define MEM_MNGR_2KB_SLOT         2
#define MEM_MNGR_FREE_SLOT        0
#define MEM_MNGR_USED_SLOT        1
#define MEM_MNGR_SLOT_SIZE     4096
#define MEM_MNGR_LAST_BLOCK       0

typedef struct {
  uint32_t base_ptr;
  uint32_t length;
  uint32_t type;
} __attribute__((packed)) mem_slot;

typedef struct {
  uint32_t kernel_location;
  uint32_t kernel_size;
  uint32_t kernel_end;
  uint32_t mem_map_start;
  uint32_t mem_map_size;
  uint32_t mem_map_end;
  uint32_t total_memory_size;
  uint64_t alloc_size; 
  uint64_t bitmap_size;
  uint32_t bitmap_addr;
} __attribute__((packed)) memory_info_t;

int memory_init(struct multiboot_header* mboot_header);

uint32_t* kmalloc(uint32_t length);
int kfree(uint32_t* base, uint32_t length);


uint64_t get_bitmap_size();
uint32_t get_bitmap_addr();

uint32_t get_kernel_location();
uint32_t get_kernel_size();
uint32_t get_kernel_end();

memory_info_t get_memory_info();
#endif