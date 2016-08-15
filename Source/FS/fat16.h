#ifndef FAT16_H
#define FAT16_H

#define FAT_16_BYTES_PER_CLUSTER           2
#define FAT_16_MAX_CLUSTERS                (65524)
#define FAT_16_MAX_FILE_SIZE               ((4*1024*1024) - 1)
#define FAT_16_MAX_FILES                   (65536)
#define FAT_16_MAX_ROOT_DIR_ENTRIES        512

#define FAT_16_FILE_NAME_NEVER_USED            0x00
#define FAT_16_FILE_NAME_USED_DELETED          0xE5
#define FAT_16_FILE_NAME_FIRST_CHARACTER_E5    0x05
#define FAT_16_FILE_NAME_DIRECTORY_ENTRY       0x2E

#define FAT_16_ATTRIBUTE_READ_ONLY             0x01
#define FAT_16_ATTRIBUTE_HIDDEN                0x02
#define FAT_16_ATTRIBUTE_SYSTEM                0x04
#define FAT_16_ATTRIBUTE_VOLUME_LABEL          0x08
#define FAT_16_ATTRIBUTE_SUBDIRECTORY          0x10
#define FAT_16_ATTRIBUTE_ARCHIVE               0x20
#define FAT_16_ATTRIBUTE_NOT_USED40            0x40
#define FAT_16_ATTRIBUTE_NOT_USED80            0x80

#define FAT_16_RESERVED_SECTORS                1

typedef struct {
  uint8_t    jmp[3];
  char       oem_name[8];
  uint16_t   bytes_per_sector;
  uint8_t    sectors_per_cluster;
  uint16_t   reserved_sectors;
  uint8_t    number_of_fats;
  uint16_t   root_entries;
  uint16_t   small_sectors;
  uint8_t    media_descriptor;
  int16_t    sectors_per_fat;
  uint32_t   hidden_sectors;
  uint32_t   large_sectors;
} __attribute__((packed)) fat16_bpb;

typedef struct {
  uint8_t   file_name[11];
  uint8_t   attributes;
  uint8_t   NT_reserved;
  uint8_t   creation_time_tenths_second;
  uint16_t  creation_time_of_day;
  uint16_t  creation_date;
  uint16_t  last_accessed_date;
  uint16_t  first_cluster_number_high_16_bits;//always zero on FAT16
  uint16_t  last_modification_time;
  uint16_t  last_modification_date;
  uint16_t  first_cluster_number_low_16_bits;
  uint32_t  file_size;
  uint8_t   reserved[3]; 
} __attribute__((packed)) fat16_directory;

typedef struct {
  uint8_t  order;
  uint8_t  first_5_characters[10];
  uint8_t  attribute;//always 0x0F
  uint8_t  long_entry_type;
  uint8_t  checksum;
  uint8_t  next_6_characters[12];
  uint16_t always_zero;
  uint8_t  last_2_characters[4];  
} __attribute__((packed)) fat16_lfn;

int fat16_format(ata_atapi_device device);

#endif