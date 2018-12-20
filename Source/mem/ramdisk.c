#include <system.h>

node_t* disk_head;

int load_ramdisk(struct multiboot_header* mboot_header)
{
  //later we will also parse the ramdisk file (if there is one in the module list)
  printf("ramdisk_init:\n");
  
  if (mboot_header->mods_count > 0)
  {
    for (int i = 0; i < (int)mboot_header->mods_count; i++)
    {
    	mboot_mod_desc_t* mod_info = (mboot_mod_desc_t*)(mboot_header->mods_addr + (i*sizeof(mboot_mod_desc_t)));
      
      printf("....module %d start : %d\n", i, mod_info->start);
      printf("....module %d end : %d\n", i, mod_info->end);
      printf("....module %d string : %d, '%s'\n", i, mod_info->string, mod_info->string);
      printf("\n");
    }
  }
  else
  {
    printf("....-no modules found, and no initrd image-\n");
  }
  
  disk_head = (node_t*)kmalloc(sizeof(node_t));
  
  /*node_t* new_file = create_rd_file(disk_head, "hi", RAMDISK_FILE);
  printf("debug dump of test rd file:\n");
  printf("directory inode:\n");
  printf(" val: %d\n", new_file->val);
  printf(" data: %d\n", (uint32_t)new_file->data);
  printf("file descriptor:\n");
  
  ramdisk_file_descriptor_t* file_desc = (ramdisk_file_descriptor_t*)new_file->data;
  
  printf(" type: %d\n", file_desc->type);
  printf(" size: %d\n", file_desc->size);
  printf(" name: %s\n", file_desc->name);
  printf(" val: %d\n", (uint32_t)file_desc->file_head->data);
  */
  //===========
  
  /*printf("\ndebug dump of rd_write - OVERWRITE:\n");
  write_rd_file(new_file, (uint8_t*)("OVERWRITE"), RAMDISK_OVERWRITE);
  printf("%s\n", (char*)file_desc->file_head->data);
  
  printf("\ndebug dump of rd_write - APPEND:\n");
  for (int i = 0; i < RAMDISK_BLOCKSIZE; i++)
  {
    write_rd_file(new_file, (uint8_t*)("----"), RAMDISK_APPEND);
  }
  printf("%s\n", (char*)file_desc->file_head->data);*/
  return K_SUCCESS;
}

node_t* create_rd_file(node_t* directory_node, char* file_name, uint32_t type)
{
  int d_index = list_add_node(directory_node, type);
  node_t* file = list_access_node(directory_node, d_index);
  
  file->data = (uint32_t*)kmalloc(sizeof(ramdisk_file_descriptor_t));
  ramdisk_file_descriptor_t* file_desc = (ramdisk_file_descriptor_t*)file->data;
  file_desc->type = type;
  file_desc->size = RAMDISK_BLOCKSIZE;
  file_desc->name = file_name;
  
  //allocate a block for the new file
  file_desc->file_head = (node_t*)kmalloc(sizeof(node_t));
  file_desc->file_head->val = 0;
  file_desc->file_head->next = NULL;
  file_desc->file_head->child = NULL;
  if (type == RAMDISK_FILE)
    file_desc->file_head->data = (uint32_t*)kmalloc(RAMDISK_BLOCKSIZE);
  else
    file_desc->file_head->data = NULL;
  return file;
}

void write_rd_file(node_t* file, uint8_t* data, int overwrite)
{
  ramdisk_file_descriptor_t* file_desc = (ramdisk_file_descriptor_t*)file->data;
  
  uint8_t* data_start = NULL;
  
  uint8_t* data_current = NULL;
  uint8_t* data_current_local = NULL;
  
  node_t* data_block = file_desc->file_head;
  int data_index = 0;
  
  data_start = (uint8_t*)data_block->data;
  if (overwrite == RAMDISK_APPEND) {
    //traverse file_head until next == NULL
    while (data_block->next != NULL)
    {
      data_block = data_block->next;
      data_start = (uint8_t*)data_block->data;
    }
    //then read the data block until we find a null terminating character. mark that as beginning of file.
    for (int i = 0; i < RAMDISK_BLOCKSIZE; i++)
    {
      //printf("i: %d data[i] = '%c'\n", (uint32_t)i, data_start[i]);
      //printf("data start: %d\n", (uint32_t)data_start);
      //printf("block start: %d\n", (uint32_t)data_block->data);
      if (data_start[i] == '\0')
      {
        data_start = (uint8_t*)(data_start + i); //the - 1 is to remove the null terminating character
        //printf("data start: %d\n", (uint32_t)data_start);
        //printf("block start: %d\n", (uint32_t)data_block->data);
        
        break;
      }
    }
  }
  if (file_desc->type == RAMDISK_DIRECTORY)
    return;
  
  //when writing make sure that when block size has been exceeded, allocate a new linked list node, 
  //and allocate a data block for it to point to. then write to it.repeat.
  data_current = data_start;
  data_current_local = (uint8_t*)(data_current - (uint8_t*)data_block->data);
  
  while(data[data_index] != '\0')
  {
    if ((uint32_t)data_current_local == RAMDISK_BLOCKSIZE)
    {
      list_add_node(file_desc->file_head, 0);
      data_block = data_block->next;
      data_block->data = (uint32_t*)kmalloc(RAMDISK_BLOCKSIZE);
      
      data_current = (uint8_t*)data_block->data;
      data_current_local = 0;
    }
    
    *data_current = data[data_index];
    
    data_index++;
    data_current++;
    data_current_local++;
  }
}

node_t* get_root_dir_node()
{
  return disk_head;
}