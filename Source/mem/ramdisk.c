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

node_t* get_root_dir_node()
{
  return disk_head;
}