#include <system.h>

ata_atapi_device devices[ATA_ATAPI_DEVICE_COUNT];
file_system file_systems[ATA_ATAPI_DEVICE_COUNT];

void fs_init()
{
  for(int i = 0; i < ATA_ATAPI_DEVICE_COUNT; i++)
  {
    int present = ata_probe_silent(i, &devices[i].nblocks, &devices[i].blocksize, devices[i].name, &devices[i].device_type, &devices[i].capacity);
    devices[i].device_num = i;
    devices[i].present = present;
    
    if (FS_DEBUG == 1)
    {
      if (present)
      {
        printf("ata/atapi device %d: %d blocks, \n%d blocksize, \n%d device type, \n%d MB blocksize\n", i, devices[i].nblocks, devices[i].blocksize, devices[i].device_type, devices[i].capacity);
      }
      else
      {
        printf("ata/atapi device %d NP\n", i);
      }
    }
  }
  printf("fs: ready\n");
}

int fs_mount(int device_num, int fs_type)
{
  //ata_probe_silent();
  if (FS_DEBUG)
    printf("fs mounting %d %d %d\n", device_num, fs_type, devices[device_num].device_type);
  
  switch(fs_type)
  {
    file_systems[device_num].device = devices[device_num]; //associate file system with device;
    file_systems[device_num].fs_type = fs_type;
    file_systems[device_num].mounted = true;
  }
  return device_num;
}

int fs_format(int device_num, int fs_type)
{
  switch(fs_type)
  {
    case FS_CLEMENT_VFS: clement_vfs_format(devices[device_num]); break;
    //case FS_VFS:                 vfs_format(devices[device_num]); break;
    //case FS_FAT:                 fat_format(devices[device_num]); break;
    case FS_FAT16:             fat16_format(devices[device_num]); break;
    //case FS_FAT32:             fat32_format(devices[device_num]); break;
    //case FS_EXT2:               ext2_format(devices[device_num]); break;
    //case FS_EXT3:               ext3_format(devices[device_num]); break;
    //case FS_EXT4:               ext4_format(devices[device_num]); break;
    default: printf("No valid file system type specified"); return K_ERROR_M;
  }
  return K_SUCCESS;
}

file_system get_fs(int fs_num)
{
  return file_systems[fs_num];
}

ata_atapi_device get_device(int dev_num)
{
  return devices[dev_num];
}