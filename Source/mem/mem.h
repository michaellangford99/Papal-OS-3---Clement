#ifndef MEM_H
#define MEM_H

#define MEM_MNGR_FREE_SLOT        0
#define MEM_MNGR_USED_SLOT        1
#define MEM_MNGR_SLOT_SIZE     4096
#define MEM_MNGR_ALIGN         4096

#define MEM_MNGR_4KSLOTS_IN_4GB (1024 * 1024)
#define MEM_MNGR_SIZE_OF_MEMORY_BITMAP (MEM_MNGR_4KSLOTS_IN_4GB / 8)


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
} __attribute__((packed)) memory_info_t;

int memory_init(struct multiboot_header* mboot_header);

uint32_t* kmalloc(uint32_t length);
uint32_t kfree(uint32_t* base, uint32_t length);
uint32_t kunfree(uint32_t* base, uint32_t length);

uint32_t get_kernel_location();
uint32_t get_kernel_size();
uint32_t get_kernel_end();

memory_info_t get_memory_info();
#endif