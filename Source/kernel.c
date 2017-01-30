#include "system.h"

void kernel_main(struct multiboot_header* mboot_header, uint32_t multiboot_magic) {

	int continue_boot = multiboot_init(mboot_header, multiboot_magic);
	if (continue_boot == K_FATAL) { /*loop forever, don't boot*/ printf("multiboot panic!!!!"); while(true) { } }

	// starts basic boot-time console
	startup_graphics_init(multiboot_get_vbe_address());
	console_init();

	//install gdt, idt, isrs and irqs
	gdt_init();
	idt_init();

	//starts memory stuff
	memory_init(multiboot_get_address());
	init_paging();

	//prepare kernel multithreading
	pm_init();

	//enable interrupts
	interrupt_unblock();

	//drivers
	clock_init();
	keyboard_init();

	//ata_init();
	//fs_init();

	//data dump
	//graphics_init(multiboot_get_vbe_address());
	//multiboot_dump(multiboot_get_address());

	//load_ramdisk(multiboot_get_address());

	//test_list();//test list.c

	/*
	uint32_t kernel_location = get_kernel_location();
  uint32_t kernel_size = get_kernel_size();
  uint32_t kernel_end = get_kernel_end();

	printf("kernel_location : 0x%x,   %d bytes,    %d KB\n", kernel_location, kernel_location, kernel_location/1024);
	printf("kernel_size     : 0x%x,   %d bytes,    %d KB\n", kernel_size, kernel_size, kernel_size/1024);
	printf("kernel_end      : 0x%x,   %d bytes,    %d KB\n", kernel_end, kernel_end, kernel_end/1024);
	*/

	//fs_format(1, FS_FAT16);

	while(true)
	{
		printf_xy(40, 20, "aabb");
		printf_xy(40, 20, "bbaa");
		printf("0");
	}
}
