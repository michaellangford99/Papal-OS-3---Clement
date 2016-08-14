#ifndef FS_H
#define FS_H

void fs_init();
int  fs_mount(int device_num, int fs_type);
int  fs_format(int device_num, int fs_type);

file_system get_fs(int fs_num);
ata_atapi_device get_device(int dev_num);

#define ATA_ATAPI_DEVICE_COUNT 4
#define FS_DEBUG               0

#endif