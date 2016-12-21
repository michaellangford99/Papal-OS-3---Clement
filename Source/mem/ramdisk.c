#include "../system.h"

node_t* disk_head;

int load_ramdisk(struct multiboot_header* mboot_header)
{
  //later we will also parse the ramdisk file (if there is one in the module list)
  printf(" ------------Loading INITRD image & ramdisk --------------\n\n");
  
  if (mboot_header->mods_count > 0)
  {
    for (int i = 0; i < (int)mboot_header->mods_count; i++)
    {
    	mboot_mod_desc_t* mod_info = (mboot_mod_desc_t*)(mboot_header->mods_addr + (i*sizeof(mboot_mod_desc_t)));
      
      printf("module %d start : %d, '%s'\n", i, mod_info->start, mod_info->start);
      printf("module %d end : %d, '%s'\n", i, mod_info->end, mod_info->end);
      printf("module %d string : %d, '%s'\n", i, mod_info->string, mod_info->string);
      printf("\n");
    }
  }
  else
  {
    printf("-no modules found, and no initrd image-\n");
  }
  
  disk_head = (node_t*)kmalloc(sizeof(node_t));
  
  node_t* new_file = create_rd_file(disk_head, "hi", RAMDISK_FILE);
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
  
  //===========
  
  printf("\ndebug dump of rd_write - OVERWRITE:\n");
  write_rd_file(new_file, (uint32_t*)("hello"), RAMDISK_OVERWRITE);
  list_print(file_desc->file_head);
  printf("%s\n", (char*)file_desc->file_head->data);
  
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
  
  file_desc->file_head->data = (uint32_t*)kmalloc(RAMDISK_BLOCKSIZE);
    
  return file;
}

void write_rd_file(node_t* file, uint32_t* data, int overwrite)
{
  ramdisk_file_descriptor_t* file_desc = (ramdisk_file_descriptor_t*)file->data;
  
  uint32_t* data_start = NULL;
  
  uint32_t* data_current = NULL;
  uint32_t* data_current_local = NULL;
  
  node_t* data_block = file_desc->file_head;
  int data_index = 0;
  
  if (overwrite == RAMDISK_OVERWRITE)
    data_start = file_desc->file_head->data;
  if (overwrite == RAMDISK_APPEND) {
    //traverse file_head until next == NULL
    while (data_block != NULL)
    {
      data_block = data_block->next;
      data_start = data_block->data;
    }
    //then read the data block until we find a null terminating character. mark that as beginning of file.
    for (int i = 0; i < RAMDISK_BLOCKSIZE; i++)
    {
      if (data_start[i] == '\0')
      {
        data_start = (uint32_t*)((uint32_t)data_start + i);
        break;
      }
    }
  }
  if (file_desc->type == RAMDISK_DIRECTORY)
    return;
  
  //when writing make sure that when block size has been exceeded, allocate a new linked list node, 
  //and allocate a data block for it to point to. then write to it.repeat.
  data_current = data_start;
  data_current_local = (uint32_t*)((uint32_t)data_current - (uint32_t)data_block->data);
  
  while(data[data_index] != '\0')
  {
    if ((uint32_t)data_current_local >= RAMDISK_BLOCKSIZE)
    {
      data_block->next = (node_t*)kmalloc(sizeof(node_t));
      data_block->child = NULL;
      data_block = data_block->next;
      data_block->data = (uint32_t*)kmalloc(RAMDISK_BLOCKSIZE);
      
      data_current = data_block->data;
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