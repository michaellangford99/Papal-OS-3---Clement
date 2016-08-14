#include "../system.h"

int fat16_format(ata_atapi_device device)
{
  printf("formatting device %d with a FAT16 file system\n", device.device_num);
  
  int capacity = device.capacity;
  int cluster_size = 0;
  
  if (capacity >= 7 && capacity <= 16){ printf("FAT16 will be formatted with a 2KB cluster size\n"); cluster_size = 2048; }
  if (capacity >= 17 && capacity <= 32){ printf("FAT16 will be formatted with a 512B cluster size\n"); cluster_size = 512; }
  if (capacity >= 33 && capacity <= 64){ printf("FAT16 will be formatted with a 1KB cluster size\n"); cluster_size = 1024; }
  if (capacity >= 65 && capacity <= 128){ printf("FAT16 will be formatted with a 2KB cluster size\n"); cluster_size = 2048; }
  if (capacity >= 129 && capacity <= 256){ printf("FAT16 will be formatted with a 4KB cluster size\n"); cluster_size = 4096; }
  if (capacity >= 257 && capacity <= 512){ printf("FAT16 will be formatted with a 8KB cluster size\n"); cluster_size = 8192; }
  if (capacity >= 513 && capacity <= 1024){ printf("FAT16 will be formatted with a 16KB cluster size\n"); cluster_size = 16384; }
  if (capacity >= 1025 && capacity <= 2048){ printf("FAT16 will be formatted with a 32KB cluster size\n"); cluster_size = 32768; }
  if (capacity >= 2049 && capacity <= 4096){ printf("FAT16 will be formatted with a 64KB cluster size\n"); cluster_size = 65536; }
  if (cluster_size == 0)
  { 
    printf("the disk is either too large\n");
    printf("or too small to be formatted \n");
    printf("as a FAT16 file system\n");
    printf("please choose a disk larger than 16 MB\n");
    printf("and smaller than or equal to 4GB\n"); 
  }
  
  if(device.blocksize != 512)
  {
    printf("this FAT16 driver does\n not support a blocksize of %d\n", device.blocksize);
    return 0;
  }
  
  //
  // format the FAT16 boot sector
  //
  
  /*
  typedef struct {
    uint8_t    jmp[3];
    uint8_t    oem_name[8];
    uint16_t   bytes_per_sector;
    uint8_t    sectors_per_cluster
    uint16_t   reserved_sectors;
    uint8_t    number_of_fats;
    uint16_t   root_entries;
    uint16_t   small_sectors;
    uint8_t    media_descriptor;
    int16_t    sectors_per_fat;
    uint32_t   hidden_sectors;
    uint32_t   large_sectors;
  } __attribute__((packed)) fat16_bpb;
  */  
  
  fat16_bpb boot_sector;
  
  boot_sector.jmp[0] = 0xEB;
  boot_sector.jmp[1] = 0xFE;
  boot_sector.jmp[2] = 0x90;
  
  memcpy(&boot_sector.oem_name[0], "clement ", 8);
  
  boot_sector.bytes_per_sector = 512;
  boot_sector.sectors_per_cluster = cluster_size / boot_sector.bytes_per_sector;
  boot_sector.reserved_sectors = FAT_16_RESERVED_SECTORS;
  boot_sector.number_of_fats = 1; // no backup FAT in  this version of the driver
  
  
  
  
  return K_SUCCESS;
  
}