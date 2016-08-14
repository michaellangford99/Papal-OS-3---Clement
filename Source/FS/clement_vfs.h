#ifndef CLEMENT_VFS_H
#define CLEMENT_VFS_H

#define CLEMENT_VFS_QUICK_FORMAT                  0

#define CLEMENT_VFS_BOOT_BLOCK_OFFSET             0
#define CLEMENT_VFS_DESCRIPTOR_BLOCK_OFFSET       1
#define CLEMENT_VFS_DATA_BITMAP_BLOCK_OFFSET      2
#define CLEMENT_VFS_INODE_BITMAP_BLOCKS_TEMP      1
#define CLEMENT_VFS_VERSION                       0

typedef uint16_t clement_vfs_descriptor;
#define CLEMENT_VFS_DESCRIPTOR_BITS               16

#define CLEMENT_VFS_DIRECTORY                     0
#define CLEMENT_VFS_FILE                          1

#define CLEMENT_VFS_RWE                           0

#define CLEMENT_VFS_KERNEL_PRIV                   10

typedef struct {
  uint8_t mode;
  uint32_t size;
  uint32_t blocks;
  uint8_t flags;
  uint16_t permissions;
  uint32_t first_block;
} clement_vfs_inode;

typedef struct {
  uint8_t in_use;
  char    name[11];
  uint16_t blocks;
  uint16_t first_block;
} __attribute__((packed)) clement_vfs_fat_entry;

int clement_vfs_format(ata_atapi_device device);
int clement_vfs_mkfile(char* name, ata_atapi_device device);
int clement_vfs_delfile(char* name, ata_atapi_device device);
int clement_vfs_write(char* name, ata_atapi_device device, char write_mode, char* buffer);

int delete_fat_entry(char* name, ata_atapi_device device);
int find_fat_entry(char* name, ata_atapi_device device);
int find_free_block(ata_atapi_device device);

#endif