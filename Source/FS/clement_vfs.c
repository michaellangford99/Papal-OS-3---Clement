#include "../system.h"

#define FAT_ENTRY_SIZE  16
#define BLOCKS_PER_FAT  8
#define BOOT_BLOCKS     1
#define START_OF_DATA   9
#define FREE_BLOCK      0
#define USED_BLOCK      9
#define LAST_BLOCK      0
#define MAX_CHARS_VFS_NAME 11

//TODO : clement_vfs_read(char* name, ata_atapi_device device, char* buffer, int length)
//TODO : clement_vfs_find_file_size(char* name, ata_atapi_device device)
//TODO : clement_vfs_defrag(ata_atapi_device device)


//fully commented and reviewed as non-buggy Vs 1
int clement_vfs_format(ata_atapi_device device) {
  printf("format device %d\n", device.device_num);
  //zero all blocks
  uint8_t zero[device.blocksize];
  for (int i = 0 ; i < device.blocksize; i++) //create a block with all values zeroed...
  {
    zero[i] = 0x00;
  }
  for(int i = 0; i < device.nblocks; i++) //loop through all blocks on disk and zero
  {
    ata_write(device.device_num, &zero, 1, i);
    iowait();
    printf_xy(console_get_xy().x, console_get_xy().y, "....%d / %d blocks zeroed...", i+1, device.nblocks);
  }
  printf("\n....zero complete\n");
  
  
  //write boot block
  uint8_t boot_block[device.blocksize * BOOT_BLOCKS];   //declare bootblock
  for (int i = 0 ; i < device.blocksize * BOOT_BLOCKS; i++) //zero bootblock container
  {
    boot_block[i] = 0x00;
  }
  boot_block[0]=0x99; // VSFS signature ;)
  iowait();
  ata_write(device.device_num, &boot_block, BOOT_BLOCKS, 0); //write boot block back to disk
  
  //write fat blocks
  //                '  fat entries in a block       '  ' blocks in the FAT'
  int fat_entries = (device.blocksize/FAT_ENTRY_SIZE) * BLOCKS_PER_FAT; //find number of possible fat entries
  
  //TODO make use less memory!!
  clement_vfs_fat_entry fat_block[fat_entries];                 //declare a buffer of all the fat entries i.e., load the entire FAT block
  for (int i = 0 ; i < device.blocksize * BLOCKS_PER_FAT; i++)  //loop through each entry
  {
    fat_block[i].in_use = 0;                                    //mark each entry as not in use
  }
  printf("....fs can contain max %d files\n", fat_entries);
  
  iowait();
  ata_write(device.device_num, &fat_block, BLOCKS_PER_FAT, BOOT_BLOCKS); //write fat block back to disk
  
  printf("format device %d done\n\n", device.device_num);
  return K_SUCCESS;
}

//fully commented and reviewed as non-buggy Vs 1
int clement_vfs_mkfile(char* name, ata_atapi_device device) {
  
  int fat_entries = (device.blocksize/FAT_ENTRY_SIZE) * BLOCKS_PER_FAT;
  
  //read in the FAT Block
  clement_vfs_fat_entry fat_block[fat_entries];                          
  ata_read(device.device_num, &fat_block, BLOCKS_PER_FAT, BOOT_BLOCKS);
  printf("mkfile '%s'\n", name);
  
  //find if the file that we are going to create already exists- if so, exit
  if (find_fat_entry(name, device) != -1){
    printf("....file %s already exists\n", name);
    printf("mkfile abort\n\n");
    return K_ERROR_M;
  }
  
  //find an open entry...
  int fat_file_entry=-1;
  for(int i = 0; i < fat_entries; i++)
  {
    if (fat_block[i].in_use == 0) //check whether this entry is free
    {
        fat_file_entry = i; //this is the free entry
        printf("....found unused fat slot %d\n", i);
        break;
    }
  }
  //if no free entries are found, stop
  if (fat_file_entry == -1)
  {
    printf("....the disk already contains the maximum number of files allowed\n\n");
    return K_ERROR_M;
  }
  
  //if a free block has been found, write to it
  fat_block[fat_file_entry].in_use = 1; //mark newly found block as used
  for (int i = 0; i < MAX_CHARS_VFS_NAME; i++) //loop through name
  {
    fat_block[fat_file_entry].name[i] = *(name + i); //enter in name
    if (*(name + i) == '\0') break; //stop when null char is found
  }
  fat_block[fat_file_entry].blocks = 1; // default size begins as one used block
  
  //find a free block
  //use 16 bits for block address numbers
  //we dont use 'find_free_block' here because of what stuff we do inside the function
  
  uint16_t data_block[device.blocksize/2]; //declare a container for datablock - it is a 16-bit variable, 
                                           //and the blocksize is in bytes, so to declare just one block, 
                                           //the blocksize is devided by two


  fat_block[fat_file_entry].first_block = -1; //set our new files fat entry for the first block to the default, -1, 
                                              //so that if it is -1 when we are finished, we know there's been an error
                                              
  for (int i = START_OF_DATA; i < device.nblocks; i++) //loop through all data blocks
  {
    ata_read(device.device_num, &data_block, 1, i);     //read in the current block we are checking
    if (data_block[0] == FREE_BLOCK)                    //if block is marked as free...
    {
      fat_block[fat_file_entry].first_block = i;        //use it as our file's first block          
      data_block[0] = USED_BLOCK;                       //mark that it is now used
      data_block[1] = LAST_BLOCK;                       //mark that it is the last block in the file
      ata_write(device.device_num, &data_block, 1, i);  //write modified block back to the disk
      ata_write(device.device_num, &fat_block, BLOCKS_PER_FAT, BOOT_BLOCKS);  //write modified fat block back to the disk
      printf("....found free data\n....block %d and marked\n....it as used\n", i);
      
      printf("mkfile '%s' complete\n\n", name);
      return K_SUCCESS;
    }
  }
  //if we are here, then a free block wasn't found
  printf("....there are no more free\n....blocks left on the disk\n\n");
  return K_ERROR_M;
}

int clement_vfs_delfile(char* name, ata_atapi_device device) {
/*  //the first step is to read in the FAT blocks and try to find the file entry...
  int fat_entry = find_fat_entry(device, name);
  //read in the fat entry to find address of first block
  int fat_block_number=0;
  int fat_entry_number_relative_to_block = fat_entry;
  if (fat_entry != 0)
  {
    fat_block_number = fat_entry / (device.blocksize/FAT_ENTRY_SIZE);
    fat_entry_number_relative_to_block = fat_entry % (device.blocksize/FAT_ENTRY_SIZE);
  }
  //read in the fat block
  clement_vfs_fat_entry fat_block[device.blocksize/FAT_ENTRY_SIZE];
  ata_read(device.device_num, &fat_block, 1, BOOT_BLOCKS + fat_block_number);
  
  int file_blocks = fat_block[fat_entry_number_relative_to_block].blocks;
  int block_numbers[file_blocks];
  block_numbers[0] = fat_block[fat_entry_number_relative_to_block].first_block;
  //next, find the first data block specified. after reading the next block number, 
  //delete the block. repeat until the LAST_BLOCK constant is found...
  for (int i = 0; i < file_blocks; i++)
  {
    uint16_t data_block[device.blocksize];                                //read in the data block
    ata_read(device.device_num, &data_block, 1, block_numbers[i]);        //..
    //if (data_block[0] != USED_BLOCK) break;                             //error
    if (data_block[1] == LAST_BLOCK) break;                               //if its the LAST_BLOCK we break before trying addressing the nect bnum
    block_numbers[i+1] = data_block[1];                                   //save block number mentioned in NEXT_BLOCK field in data block  
  }
  for (int i = 0; i < file_blocks; i++)
  {
    uint16_t data_block[device.blocksize];                                
    ata_read(device.device_num, &data_block, 1, block_numbers[i]);        
    data_block[0] = FREE_BLOCK;                             
    data_block[1] = 0;   
    ata_write(device.device_num, &data_block, 1, block_numbers[i]);        //..
  }
  //then, delete the FAT entry...
  fat_block[fat_entry_number_relative_to_block].in_use = 0;
  for (int i = 0; i < MAX_CHARS_VFS_NAME; i++)
  {
    fat_block[fat_entry_number_relative_to_block].name[i] = 0;
  }
  fat_block[fat_entry_number_relative_to_block].blocks = 0;
  fat_block[fat_entry_number_relative_to_block].first_block = 0;
  ata_write(device.device_num, &fat_block, 1, BOOT_BLOCKS + fat_block_number);*/
  return K_SUCCESS;
}

int clement_vfs_write(char* name, ata_atapi_device device, char write_mode, char* buffer) {
  printf("write file %s\n", name);
  
  //find which entry is used by the file
  int fat_entry_number = find_fat_entry(name, device);
  
  //check whether a file was found
  if (fat_entry_number == -1)
  {
    printf("....file %s not found\n", name);
    printf("write abort\n");
    return K_ERROR_M;
  }
  
  //So this is kind of confusing... We know which fat ENTRY the file is in, but we don't know 
  //what fat BLOCK (512 bytes, contains a bunch of fat entries) our file is in.
  //Now we must find which fat BLOCK the file is in
  int fat_block_number=0;  // THIS IS RELATIVE TO THE FIRST FAT BLOCK!!!!!!
  //fat_entry_number_relative_to_block is the entry number relative to the block that the entry is in - say, entry 5, in block 64. the full number would be entry 2053
  int fat_entry_number_relative_to_block = 0;
  
  
  if (fat_entry_number != 0)//check whether it is entry zero, to stop division by zero error
  {
    fat_block_number = fat_entry_number / (device.blocksize/FAT_ENTRY_SIZE);
    fat_entry_number_relative_to_block = fat_entry_number % (device.blocksize/FAT_ENTRY_SIZE);
  }
  printf("....entry is in block %d, entry %d\n", fat_block_number + BOOT_BLOCKS, fat_entry_number_relative_to_block);
  
  //read in the fat block containing our file's fat entry
  clement_vfs_fat_entry fat_block[device.blocksize/FAT_ENTRY_SIZE];
  ata_read(device.device_num, &fat_block, 1, BOOT_BLOCKS + fat_block_number);
  
  //find amount of blocks used by file
  int file_blocks = fat_block[fat_entry_number_relative_to_block].blocks;
  printf("....there are %d blocks in %s\n", file_blocks, name);
  
  //declare container for block numbers of the file's blocks
  int file_block_numbers[file_blocks];
  
  //set first block number as the block number of the first data block in the file
  file_block_numbers[0] = fat_block[fat_entry_number_relative_to_block].first_block;
  
  //find first block. if it has more blocks in the list, keep navigating htrough untill the LAST_BLOCK
  // is found. then, if APPEND, write from there. if not, mark all but the first block as free and continue 
  //writing from the beginning
  for (int i = 0; i < file_blocks; i++) //loop through blocks in file, getting their numbers - like traversing a linked list
  {
    uint16_t data_block[device.blocksize];                                  //read in the data block
    ata_read(device.device_num, &data_block, 1, file_block_numbers[i]);     //..
    //if (data_block[0] != USED_BLOCK) break;                               //error
    if (data_block[1] == LAST_BLOCK) break;                                 //if its the LAST_BLOCK we break before trying addressing the nect block num
    file_block_numbers[i+1] = data_block[1];                                //save block number mentioned in NEXT_BLOCK field in data block  
  }
  
  if (write_mode == FILE_OVERWRITE)         //if overwriting the file, we must first mark the first data bock as the last block
  {                                         //and then erase the other data blocks
    printf("....write mode is overwrite, \n....now deleting all but one block\n");
    uint16_t first_data_block[device.blocksize];
    ata_read(device.device_num, &first_data_block, 1, file_block_numbers[0]); //read in first data block
    first_data_block[1] = LAST_BLOCK;                                         //change next block number to LAST_BLOCK
    ata_write(device.device_num, &first_data_block, 1, file_block_numbers[0]);//write back the data block
    printf("....marked first block in file \n....as the last block\n");
    
    for (int i = 1; i < file_blocks; i++)                                   //start with the second block
    {
      printf("deleting block %d in list, #%d\n", i, file_block_numbers[i]);
      delete_data_block(file_block_numbers[i], device);                     //delete the block, using subfunction
    }
    
    int blocks_in_new_file = strlen(buffer)/(device.blocksize-4);           //find number of blocks needed to fit data. The first 32 bits of 
                                                                            //each block are reserved for metadata
    if (blocks_in_new_file == 0) blocks_in_new_file = 1;                    //if data is smaller than what will fit in one block, 
                                                                            //the 'blocks needed' rounds down to zero. to fix this, there is a catch

    printf("....file will occupy %d blocks\n", blocks_in_new_file);
                                                                            
    //loop through each block to be written
    for(int i = 0; i < blocks_in_new_file; i++)                //write one block at a time
    {
      char dat[device.blocksize];                              //declare the block
      int block_number = find_free_block(device);              //find the next free block
      printf("....the next free block is %d\n", block_number);
      if (i == 0)                                              //if we are on the first block, we want to write to the first block of the old file
      {
        block_number = file_block_numbers[0];                  //if we are writing the first block, use the block number referred to in FAT
        printf("....the first block in file is %d\n", block_number);
      }
      ata_read(device.device_num, &dat, 1, block_number);      //read in the block
      dat[0] = USED_BLOCK;                                     //mark it as used
      ata_write(device.device_num, &dat, 1, block_number);     //write block to disk
      dat[2] = find_free_block(device);                        //mark next block
      dat[3] = find_free_block(device) >> 8;                   //1 byte at a time  
      for (int i = 4; i < device.blocksize; i++)               //loop through the block, starting at 4 (first 32 bits of data block are reserved)
      {
        dat[i] = *buffer;                                      //copy the data from buffer
        if (*buffer == '\0')                                   //stop after finding the EOS char
        {
          dat[2] = LAST_BLOCK;                                 //block has already been marked as used, what is left is to mark
          dat[3] = LAST_BLOCK >> 8;                            //the second 16-bit value as the last block ( 0000 )
          printf("....finished writing buffer %d\n", i);
          printf("....now updating FAT entry\n");
          fat_block[fat_entry_number_relative_to_block].blocks = blocks_in_new_file;  //modify FAT entry to state the nmber of blocks in the file
          ata_write(device.device_num, &fat_block, 1, BOOT_BLOCKS + fat_block_number);//actually write the FAT block back to the disk
          break;
        }
        buffer++;                                              //increase pointer to next char
      }
      ata_write(device.device_num, &dat, 1, block_number);     //write data block to disk
    }
  }
  printf("write done\n\n");
  return K_SUCCESS;
}

//fully commented and reviewed as non-buggy Vs 1
int delete_fat_entry(char* name, ata_atapi_device device) {
  printf("delete_fat_entry '%s' on device %d\n", name, device.device_num);
  /*
  To delete a FAT entry, first we have to find the absolute (relative to the first entry) entry number of the file.
  Then, we must find its entry number relative to the block it is in.
  Next, we clear the values in the FAT entry - it is now gone forever!
  */
  int absolute_entry_number = find_fat_entry(name, device);                         //find entry number
  if (absolute_entry_number == -1)                                                  //check that a file entry was found
  {
    printf("....file '%s' not found\n", name);
    printf("delete_fat_entry abort\n\n");
    return -1;
  }
  
  int fat_block_number=0;                                                           //block that fat entry in in
  int fat_entry_number_relative_to_block = 0;                                       // fat entry in block
  
  if (absolute_entry_number != 0)                                                   //check whether it is entry zero, to stop division by zero error
  {
    fat_block_number = absolute_entry_number / (device.blocksize/FAT_ENTRY_SIZE);
    fat_entry_number_relative_to_block = absolute_entry_number % (device.blocksize/FAT_ENTRY_SIZE);
  }
  printf("....entry is in block %d, entry %d\n", BOOT_BLOCKS + fat_block_number, fat_entry_number_relative_to_block);
  
  clement_vfs_fat_entry fat_block[device.blocksize/FAT_ENTRY_SIZE];                 //declare container for FAT block
  ata_read(device.device_num, &fat_block, 1, BOOT_BLOCKS + fat_block_number);       //read in FAT block
  
  memset((char*)&fat_block[fat_entry_number_relative_to_block], 0, FAT_ENTRY_SIZE); //clear the FAT block
  
  ata_write(device.device_num, &fat_block, 1, BOOT_BLOCKS + fat_block_number);      //write the FAT block back to the disk
  
  printf("delete_fat_entry done\n\n");
  return K_SUCCESS;
}

//fully commented and reviewed as non-buggy Vs 1
int delete_data_block(int absolute_block_number, ata_atapi_device device) {
  if (absolute_block_number >= device.nblocks)                         //check that the desired block to be deleted is actually on the disk
  {
    printf("ERROR -> delete_data_block -> data block %d does not exist\n", absolute_block_number);
    return K_ERROR_M;
  }
  
  uint8_t zero_block[device.blocksize];                                //declare zero block
  memset((char*)&zero_block[0], 0, device.blocksize);                  //set entire block to zeroes
  ata_write(device.device_num, &zero_block, 1, absolute_block_number); //write block back to disk
  
  return K_SUCCESS;
}

//fully commented and reviewed as non-buggy Vs 1
int find_fat_entry(char* name, ata_atapi_device device) {
  int fat_entries = (device.blocksize/FAT_ENTRY_SIZE) * BLOCKS_PER_FAT;    //declare number of max fat entries
  clement_vfs_fat_entry fat_block[fat_entries];                            //container for fat table that is currently being searched
  ata_read(device.device_num, &fat_block, BLOCKS_PER_FAT, BOOT_BLOCKS);    //read in the fat table
  
  for(int i = 0; i < fat_entries; i++)                                     //loop through every entry
  {
    if (strcmp(name, &(fat_block[i].name[0])) == 0)                        //if the names for the entries are the same...
    {
      if (fat_block[i].in_use == 1)                                        //and the entry is in use..
      {
        printf("....found FAT entry %d containing file name\n", i);        //we have found our fat entry!
        return i;
      }
      else{
        printf("WARNING -> find_fat_entry -> found orphaned fat entry %d", i);
      }
    }
  }
  return -1;
}

//fully commented and reviewed as non-buggy Vs 1
int find_free_block(ata_atapi_device device) {
  uint16_t data_block[device.blocksize/2];              //declare our data buffer
  for (int i = START_OF_DATA; i < device.nblocks; i++)  //loop through the data blocks
  {
    ata_read(device.device_num, &data_block, 1, i);     //read in the current block we are checking
    if (data_block[0] == FREE_BLOCK)                    //if block is marked as free...
    {
      return i;                                         //return its block number
    }
  }
  printf("ERROR -> find_free_block -> no free blocks were found on device %d\n", device.device_num);
  return 0;
}