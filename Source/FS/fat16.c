#include "../system.h"

//to be used later :
/*printf("formatting device %d with a FAT16 file system\n", device.device_num);

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


//typedef struct {
//  uint8_t    jmp[3];
//  uint8_t    oem_name[8];
//  uint16_t   bytes_per_sector;
//  uint8_t    sectors_per_cluster
//  uint16_t   reserved_sectors;
//  uint8_t    number_of_fats;
//  uint16_t   root_entries;
//  uint16_t   small_sectors;
//  uint8_t    media_descriptor;
//  int16_t    sectors_per_fat;
//  uint32_t   hidden_sectors;
//  uint32_t   large_sectors;
//} __attribute__((packed)) fat16_bpb;
 

fat16_bpb boot_sector;

boot_sector.jmp[0] = 0xEB;
boot_sector.jmp[1] = 0xFE;
boot_sector.jmp[2] = 0x90;

memcpy(&boot_sector.oem_name[0], "clement ", 8);

boot_sector.bytes_per_sector = 512;
boot_sector.sectors_per_cluster = cluster_size / boot_sector.bytes_per_sector;
boot_sector.reserved_sectors = FAT_16_RESERVED_SECTORS;
boot_sector.number_of_fats = 1; // no backup FAT in  this version of the driver
*/

int fat16_format(ata_atapi_device device)
{
  
  //read in partition table
  PartitionBlock pb;
  ata_read(device.device_num, &pb, 1, 0);
  
  //dump contents of partition table
  int i;
  for(i=0; i<4; i++)
  {
      printf("Partition %d:\n  type: %x\n", i, pb.pt[i].partition_type);
      printf("  Start sector: %x\n  %d sectors long\n", 
          pb.pt[i].start_sector, pb.pt[i].length_sectors);
  }

  //determine whether partition table contains FAT16 partition
  for (i = 0; i < 4; i++)
  {
    if (pb.pt[i].partition_type == 6)
    {
      printf("Partition %d contains a FAT16 partition!", i);
      break;
    }
  }
  
  //read in FAT16 boot sector
  Fat16BootSector boot_sector;
  ata_read(device.device_num, &boot_sector, 1, pb.pt[i].start_sector);
  
  //dump contents of boot sector
  printf("jmp: 0x%x 0x%x 0x%x\n", boot_sector.jmp[0], boot_sector.jmp[1], boot_sector.jmp[2]);
  printf("oem: %s\n", &boot_sector.oem[0]);
  printf("sector size: %d\n", (int32_t)boot_sector.sector_size);
  printf("sectors per cluster: %d\n", (int32_t)boot_sector.sectors_per_cluster);
  printf("reserved sectors: %d\n", (int32_t)boot_sector.reserved_sectors);
  printf("num of FATs: %d\n", (int32_t)boot_sector.number_of_fats);
  printf("root directory entries: %d\n", (int32_t)boot_sector.root_dir_entries);
  printf("total sectors: %d\n", (int32_t)boot_sector.total_sectors_short); // if zero, later field is used
  printf("media_descriptor: %d\n", (int32_t)boot_sector.media_descriptor);
  printf("fat size (in sectors): %d\n", (int32_t)boot_sector.fat_size_sectors);
  printf("total sectors: %d\n", (int32_t)boot_sector.total_sectors_long);
  printf("volume_id: %d\n", (int32_t)boot_sector.volume_id);
  //printf("volume_label: %s\n", &boot_sector.volume_label[0]);
  //printf("fs_type: %s\n", &boot_sector.fs_type[0]);
  //printf("boot code: %s\n", &boot_sector.boot_code[0]);
  //printf("boot sector signature: %d\n", boot_sector.boot_sector_signature);
  
  //read in root directory
  int root_dir_sectors = (((int32_t)boot_sector.root_dir_entries * 32) + ((int32_t)boot_sector.sector_size - 1)) / (int32_t)boot_sector.sector_size;
  printf("root_dir_sectors: %d\n", (int32_t)root_dir_sectors);
  int fat_size = (int32_t)boot_sector.fat_size_sectors;
  printf("fat size: %d\n", (int32_t)fat_size);
  int first_data_sector = (int32_t)boot_sector.reserved_sectors + ((int32_t)boot_sector.number_of_fats * (int32_t)fat_size) + (int32_t)root_dir_sectors;
  printf("first_data_sector: %d\n", (int32_t)first_data_sector);
  int first_root_dir_sector = (int32_t)first_data_sector - (int32_t)root_dir_sectors;
  printf("first root directory sector: %d\n", (int32_t)first_root_dir_sector);
  
  printf("calculation: %d\n", (int)((uint32_t)boot_sector.reserved_sectors-1 + (uint32_t)boot_sector.fat_size_sectors * (uint32_t)boot_sector.number_of_fats));
  
  fat16_root_dir_entry root_dir[(int32_t)boot_sector.root_dir_entries];
  ata_read(device.device_num, &root_dir[0], root_dir_sectors, first_root_dir_sector + pb.pt[i].start_sector);
  
  for (int j = 0; j < (int32_t)boot_sector.root_dir_entries; j++)
  {
    print_file_info(device, &root_dir[j], pb, i, boot_sector);
  }
  
  
  return K_SUCCESS;
  
}

void print_file_info(ata_atapi_device device, fat16_root_dir_entry *entry, PartitionBlock pb, int partition, Fat16BootSector boot_sector) {
    switch(entry->filename[0]) {
    case 0x00:
        return; // unused entry
    case 0xE5:
        printf("Deleted file: [");
        for (int k = 0; k < 8; k++)
        {
          printf_putchar(entry->filename[k]);
        }
        printf(".");
        for (int k = 0; k < 3; k++)
        {
          printf_putchar(entry->ext[k]);
        }
        printf("]\n");
        return;
    case 0x05:
        printf("File starting with 0xE5: [");
        for (int k = 0; k < 8; k++)
        {
          printf_putchar(entry->filename[k]);
        }
        printf(".");
        for (int k = 0; k < 3; k++)
        {
          printf_putchar(entry->ext[k]);
        }
        printf("]\n");
        break;
    case 0x2E:
        printf("Directory: [");
        for (int k = 0; k < 8; k++)
        {
          printf_putchar(entry->filename[k]);
        }
        printf(".");
        for (int k = 0; k < 3; k++)
        {
          printf_putchar(entry->ext[k]);
        }
        printf("]\n");
        break;
    default:
        printf("File: [");
        for (int k = 0; k < 8; k++)
        {
          if (entry->filename[k] > 41)
            printf_putchar(entry->filename[k]);
          
        }
        printf(".");
        for (int k = 0; k < 3; k++)
        {
          printf_putchar(entry->ext[k]);
        }
        printf("]\n");
    }
    
    printf("  Modified: %d-%d-%d %d:%d.%d    Start: [%x]    Size: %d\n", 
        1980 + (entry->modify_date >> 9), (entry->modify_date >> 5) & 0xF, entry->modify_date & 0x1F,
        (entry->modify_time >> 11), (entry->modify_time >> 5) & 0x3F, entry->modify_time & 0x1F,
        entry->starting_cluster, entry->file_size);
    
    if (entry->file_size == (uint32_t)-1)
      return;
        
    //print out first block of file:
    int root_dir_sectors = (((int32_t)boot_sector.root_dir_entries * 32) + ((int32_t)boot_sector.sector_size - 1)) / (int32_t)boot_sector.sector_size;
    int fat_size = (int32_t)boot_sector.fat_size_sectors;
    int first_data_sector = (int32_t)boot_sector.reserved_sectors + ((int32_t)boot_sector.number_of_fats * (int32_t)fat_size) + (int32_t)root_dir_sectors;
    
    int datablock = ((entry->starting_cluster-2)*(int32_t)boot_sector.sectors_per_cluster) + first_data_sector;
    printf("first block of data in file : 0x%x / %d\n", datablock, datablock);
    
    uint8_t data_buffer[(int32_t)boot_sector.sectors_per_cluster*512];
    ata_read(device.device_num, &data_buffer, (int32_t)boot_sector.sectors_per_cluster,  + pb.pt[partition].start_sector + datablock);
    for (int i = 0; i < (int32_t)boot_sector.sectors_per_cluster*512; i++)
    {
      
      printf_putchar(data_buffer[i]);
      if (data_buffer[i] == 0) { break; }
    }
}