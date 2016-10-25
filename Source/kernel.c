#include "system.h"

void kernel_main(struct multiboot_header* mboot_header, uint32_t multiboot_magic) {
	
	int continue_boot = multiboot_init(mboot_header, multiboot_magic);
	if (continue_boot == K_FATAL) { /*loop forever, don't boot*/ printf("multiboot panic!!!!"); while(true) { } }
	
	
	memory_init(multiboot_get_address());              //. gotta be first
	
	graphics_init(multiboot_get_vbe_address());        //basic initial boot-time drivers
	console_init();                                    //.
	
	//start basic drivers
	gdt_init();                                        //.
	idt_init();                                        //.
	clock_init();                                      //.
	keyboard_init();                                   //.
	
	//init paging
	init_paging();
	
	//start higher level drivers
	ata_init();                                        //.
	fs_init();                                         //.
	
	//data dump
	multiboot_dump(multiboot_get_address());           //.
	
	/*
	uint32_t kernel_location = get_kernel_location();
  uint32_t kernel_size = get_kernel_size();
  uint32_t kernel_end = get_kernel_end();
	
	printf("kernel_location : 0x%x,   %d bytes,    %d KB\n", kernel_location, kernel_location, kernel_location/1024);
	printf("kernel_size     : 0x%x,   %d bytes,    %d KB\n", kernel_size, kernel_size, kernel_size/1024);
	printf("kernel_end      : 0x%x,   %d bytes,    %d KB\n", kernel_end, kernel_end, kernel_end/1024);
	
	//fs_format(1, FS_FAT16);
	*/
	while(true)
	{
		
	}
}
