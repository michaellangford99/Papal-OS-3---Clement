#ifndef FS_TYPES_H
#define FS_TYPES_H

#define ATAPI_CDROM     0
#define ATA_HDD         1
#define ATA_FLOPPY      2
#define ATA_CASSETTE    4
#define RAM_DISK        5

typedef struct {
  int device_type;
  int device_num;
  
	char name[256];
  
  int nblocks;
  int blocksize;
  int capacity;
  
  bool present;
} ata_atapi_device;

typedef struct {
  ata_atapi_device device;
  bool mounted;  
  int fs_type;  
} file_system;

#define FS_CLEMENT_VFS  0
#define FS_VFS          1
#define FS_FAT          2
#define FS_FAT16        3
#define FS_FAT32        4
#define FS_EXT2         5
#define FS_EXT3         6
#define FS_EXT4         7

#endif