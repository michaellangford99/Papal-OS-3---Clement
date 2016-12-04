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
  
  return K_SUCCESS;
}

