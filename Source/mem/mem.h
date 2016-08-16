#ifndef MEM_H
#define MEM_H

#define MEM_MNGR_1KB_SLOT         1
#define MEM_MNGR_2KB_SLOT         2
#define MEM_MNGR_FREE_SLOT        0
#define MEM_MNGR_USED_SLOT        1
#define MEM_MNGR_SLOT_SIZE     1024
#define MEM_MNGR_LAST_BLOCK       0

typedef struct {
  uint32_t base_ptr;
  uint32_t length;
  uint32_t type;
} __attribute__((packed)) mem_slot;

int memory_init(struct multiboot_header* mboot_header);

uint32_t* kmalloc(uint32_t length);
int kfree(uint32_t* base, uint32_t length);


int get_bitmap_size();
int get_bitmap_addr();

#endif