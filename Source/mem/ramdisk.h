#ifndef RAMDISK_H
#define RAMDISK_H

typedef struct {
  uint32_t* base_ptr;
  uint32_t root_entries;
} __attribute__((packed)) ramdisk_descriptor;

typedef struct {
  uint32_t type;
  uint32_t size;
  char* name;
  
  node_t* file_head;
  /* more coming */
} __attribute__((packed)) ramdisk_file_descriptor;

int load_ramdisk(struct multiboot_header* mboot_header);

#endif