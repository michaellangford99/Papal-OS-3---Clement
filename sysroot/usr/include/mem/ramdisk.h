#ifndef RAMDISK_H
#define RAMDISK_H

#define RAMDISK_BLOCKSIZE 4096
#define RAMDISK_FILE      9
#define RAMDISK_DIRECTORY 1

#define RAMDISK_FILE_R    0
#define RAMDISK_FILE_W    1
#define RAMDISK_FILE_E    2
#define RAMDISK_FILE_RW   3
#define RAMDISK_FILE_RE   4
#define RAMDISK_FILE_WE   5
#define RAMDISK_FILE_RWE  6

#define RAMDISK_OVERWRITE  1
#define RAMDISK_APPEND     0

typedef struct {
  uint32_t* base_ptr;
  uint32_t root_entries;
} __attribute__((packed)) ramdisk_descriptor_t;

typedef struct {
  uint32_t type;
  uint32_t size;//in bytes
  char* name;
  
  node_t* file_head;
  /* more coming */
} __attribute__((packed)) ramdisk_file_descriptor_t;

int load_ramdisk(struct multiboot_header* mboot_header);
node_t* create_rd_file(node_t* directory_node, char* file_name, uint32_t type);
void write_rd_file(node_t* file, uint8_t* data, int overwrite);
node_t* get_root_dir_node();

#endif