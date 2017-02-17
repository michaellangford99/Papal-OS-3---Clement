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
	//init_paging();

	//prepare kernel multithreading
	pm_init();

	//enable interrupts
	interrupt_unblock();

	//drivers
	clock_init();
	//keyboard_init();

	//ata_init();
	//fs_init();

	//data dump
	//graphics_init(multiboot_get_vbe_address());
	//multiboot_dump(multiboot_get_address());

	//load_ramdisk(multiboot_get_address());
	
	//pm_new_thread();
	
	//console_reset();
	//test_list();
	
	/*
	uint32_t kernel_location = get_kernel_location();
  uint32_t kernel_size = get_kernel_size();
  uint32_t kernel_end = get_kernel_end();

	printf("kernel_location : 0x%x,   %d bytes,    %d KB\n", kernel_location, kernel_location, kernel_location/1024);
	printf("kernel_size     : 0x%x,   %d bytes,    %d KB\n", kernel_size, kernel_size, kernel_size/1024);
	printf("kernel_end      : 0x%x,   %d bytes,    %d KB\n", kernel_end, kernel_end, kernel_end/1024);
	*/

	//fs_format(1, FS_FAT16);

	pm_new_thread((uint32_t*)thread_2, 1024);
	pm_new_thread((uint32_t*)thread_3, 1024);
	pm_new_thread((uint32_t*)thread_4, 1024);

	while(true)
	{
		printf("year: %d\n", (uint32_t)rtc_get_year());
		printf("month: %d\n", (uint32_t)rtc_get_month());
		printf("day: %d\n", (uint32_t)rtc_get_day());
		printf("weekday: %d\n", (uint32_t)rtc_get_weekday());
		printf("hour: %d\n", (uint32_t)rtc_get_hour());
		printf("minute: %d\n", (uint32_t)rtc_get_minute());
		printf("second: %d\n", (uint32_t)rtc_get_second());
	}
}

void thread_2()
{
	while(true)
	{
		printf("1\n");
	}
}

void thread_3()
{
	while(true)
	{
		printf("2\n");
	}
}

void thread_4()
{
	while(true)
	{
		printf("3\n");
	}
}
