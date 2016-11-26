#include "system.h"

struct multiboot_header cp_mboot_header;
vbe_info_t cp_vbe_info;

int multiboot_init(struct multiboot_header* mboot_header, uint32_t multiboot_magic) {
  //first check if the magic number is valid...
  if (multiboot_magic != MULTIBOOT_KERNEL_MAGIC)
  {
    return K_FATAL;
  }
  //copy the header to the new declared location...
  memcpy((char*)(&cp_mboot_header), (char*)(mboot_header), sizeof(cp_mboot_header));
  memcpy((char*)(&cp_vbe_info), (char*)(cp_mboot_header.vbe_mode_info), sizeof(cp_vbe_info));
  
  cp_mboot_header.vbe_mode_info = (uint32_t) &cp_vbe_info;
  
  return K_SUCCESS;
}
struct multiboot_header* multiboot_get_address() {
  return &cp_mboot_header;
}
vbe_info_t* multiboot_get_vbe_address() {
  return &cp_vbe_info;
}
void multiboot_dump(struct multiboot_header* mboot_header) {
  printf("mboot header: \n");
	
	printf("flags:             0x%x / %d\n", mboot_header->flags, mboot_header->flags);
	printf("mem_lower:         0x%x / %d\n", mboot_header->mem_lower, mboot_header->mem_lower);
	printf("mem_upper:         0x%x / %d\n", mboot_header->mem_upper, mboot_header->mem_upper);
	printf("boot_device:       0x%x / %d\n", mboot_header->boot_device, mboot_header->boot_device);
	printf("cmdline:           0x%x / %d\n", mboot_header->cmdline, mboot_header->cmdline);
	printf("mods_count:        0x%x / %d\n", mboot_header->mods_count, mboot_header->mods_count);
	printf("mods_addr:         0x%x / %d\n", mboot_header->mods_addr, mboot_header->mods_addr);
  
	//attempt to print as string first module
  
  if (mboot_header->mods_count > 0)
  {
  	mboot_mod_desc_t* mod_info = (mboot_mod_desc_t*) mboot_header->mods_addr;
    
    printf("module 1 start : %d, '%s'\n", mod_info->start, mod_info->start);
    printf("module 1 end : %d, '%s'\n", mod_info->end, mod_info->end);
    printf("module 1 string : %d, '%s'\n", mod_info->string, mod_info->string);
    printf("module 1 reserved : %d\n", mod_info->start, mod_info->start);
  }
  
	printf("num:               0x%x / %d\n", mboot_header->num, mboot_header->num);
	printf("size:              0x%x / %d\n", mboot_header->size, mboot_header->size);
	printf("addr:              0x%x / %d\n", mboot_header->addr, mboot_header->addr);
	printf("shndx:             0x%x / %d\n", mboot_header->shndx, mboot_header->shndx);
	printf("mmap_length:       0x%x / %d\n", mboot_header->mmap_length, mboot_header->mmap_length);
	printf("mmap_addr:         0x%x / %d\n", mboot_header->mmap_addr, mboot_header->mmap_addr);
	printf("drives_length:     0x%x / %d\n", mboot_header->drives_length, mboot_header->drives_length);
	printf("drives_addr:       0x%x / %d\n", mboot_header->drives_addr, mboot_header->drives_addr);
	printf("config_table:      0x%x / %d\n", mboot_header->config_table, mboot_header->config_table);
	printf("boot_loader_name:  0x%x - '%s'\n", mboot_header->boot_loader_name, mboot_header->boot_loader_name);
	printf("apm_table:         0x%x / %d\n", mboot_header->apm_table, mboot_header->apm_table);
	printf("vbe_control_info:  0x%x / %d\n", mboot_header->vbe_control_info, mboot_header->vbe_control_info);
	printf("vbe_mode_info:     0x%x / %d\n", mboot_header->vbe_mode_info, mboot_header->vbe_mode_info);
	printf("vbe_mode:          0x%x / %d\n", mboot_header->vbe_mode, mboot_header->vbe_mode);
	printf("vbe_interface_seg: 0x%x / %d\n", mboot_header->vbe_interface_seg, mboot_header->vbe_interface_seg);
	printf("vbe_interface_off: 0x%x / %d\n", mboot_header->vbe_interface_off, mboot_header->vbe_interface_off);
	printf("vbe_interface_len: 0x%x / %d\n", mboot_header->vbe_interface_len, mboot_header->vbe_interface_len);
	
	vbe_info_t* vbe_header = (vbe_info_t*) mboot_header->vbe_mode_info;
	
	printf("\nvbe header: \n");
	printf("attributes:             0x%x / %d\n", vbe_header->attributes, vbe_header->attributes); 
	printf("winA:                   0x%x / %d\n", vbe_header->winA, vbe_header->winA);
	printf("winB:                   0x%x / %d\n", vbe_header->winB, vbe_header->winB); 
	printf("granularity:            0x%x / %d\n", vbe_header->granularity, vbe_header->granularity); 
	printf("winsize:                0x%x / %d\n", vbe_header->winsize, vbe_header->winsize); 
	printf("segmentA:               0x%x / %d\n", vbe_header->segmentA, vbe_header->segmentA); 
	printf("segmentB:               0x%x / %d\n", vbe_header->segmentB, vbe_header->segmentB); 
	printf("realFctPtr:             0x%x / %d\n", vbe_header->realFctPtr, vbe_header->realFctPtr); 
	printf("pitch:                  0x%x / %d\n", vbe_header->pitch, vbe_header->pitch); 
	printf("Xres:                   0x%x / %d\n", vbe_header->Xres, vbe_header->Xres); 
	printf("Yres:                   0x%x / %d\n", vbe_header->Yres, vbe_header->Yres); 
	printf("Wchar:                  0x%x / %d\n", vbe_header->Wchar, vbe_header->Wchar); 
	printf("Ychar:                  0x%x / %d\n", vbe_header->Ychar, vbe_header->Ychar); 
	printf("planes:                 0x%x / %d\n", vbe_header->planes, vbe_header->planes); 
	printf("bpp:                    0x%x / %d\n", vbe_header->bpp, vbe_header->bpp); 
	printf("banks:                  0x%x / %d\n", vbe_header->banks, vbe_header->banks); 
	printf("memory_model:           0x%x / %d\n", vbe_header->memory_model, vbe_header->memory_model); 
	printf("bank_size:              0x%x / %d\n", vbe_header->bank_size, vbe_header->bank_size); 
	printf("image_pages:            0x%x / %d\n", vbe_header->image_pages, vbe_header->image_pages); 
	printf("reserved0:              0x%x / %d\n", vbe_header->reserved0, vbe_header->reserved0); 
	printf("red_mask:               0x%x / %d\n", vbe_header->red_mask, vbe_header->red_mask); 
	printf("red_position:           0x%x / %d\n", vbe_header->red_position, vbe_header->red_position); 
	printf("green_mask:             0x%x / %d\n", vbe_header->green_mask, vbe_header->green_mask); 
	printf("blue_mask:              0x%x / %d\n", vbe_header->blue_mask, vbe_header->blue_mask); 
	printf("blue_position:          0x%x / %d\n", vbe_header->blue_position, vbe_header->blue_position); 
	printf("rsv_mask:               0x%x / %d\n", vbe_header->rsv_mask, vbe_header->rsv_mask); 
	printf("rsv_position:           0x%x / %d\n", vbe_header->rsv_position, vbe_header->rsv_position); 
	printf("directcolor_attributes: 0x%x / %d\n", vbe_header->directcolor_attributes, vbe_header->directcolor_attributes); 
	printf("physbase:               0x%x / %d\n", vbe_header->physbase, vbe_header->physbase); 
	printf("reserved1:              0x%x / %d\n", vbe_header->reserved1, vbe_header->reserved1); 
	printf("reserved2:              0x%x / %d\n", vbe_header->reserved2, vbe_header->reserved2); 
}