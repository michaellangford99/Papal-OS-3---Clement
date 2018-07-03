#include <system.h>

extern uint32_t Lhang;
extern uint32_t user_mode_function;
extern uint32_t user_mode_function2;
extern void run_user_mode();

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
	heap_init();
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
	graphics_init(multiboot_get_vbe_address());
	//multiboot_dump(multiboot_get_address());

	load_ramdisk(multiboot_get_address());


	uint32_t kernel_location = get_kernel_location();
  uint32_t kernel_size = get_kernel_size();
  uint32_t kernel_end = get_kernel_end();

	printf("kernel_location : 0x%x,   %d bytes,    %d KB\n", kernel_location, kernel_location, kernel_location/1024);
	printf("kernel_size     : 0x%x,   %d bytes,    %d KB\n", kernel_size, kernel_size, kernel_size/1024);
	printf("kernel_end      : 0x%x,   %d bytes,    %d KB\n", kernel_end, kernel_end, kernel_end/1024);

	printf("hang: 0x%x\n", &Lhang);
	printf("user: 0x%x\n", &run_user_mode);
	printf("ufunc:0x%x\n", &user_mode_function);
	printf("idt:  0x%x\n", &isr_handler);
	printf("isr:  0x%x\n", &irq_handler);
	printf("main: 0x%x\n", &kernel_main);
	printf("proc: 0x%x\n", &proc_save);
	//graphics_update_fb();

	//run_user_mode();

	//fs_format(1, FS_FAT16);
	pm_new_thread((uint32_t*)&user_mode_function, 1024, PM_PL3);
	pm_new_thread((uint32_t*)&user_mode_function2, 1024, PM_PL3);
	/*pm_new_thread((uint32_t*)thread_3, 1024);
	pm_new_thread((uint32_t*)thread_4, 1024);
  */

	//dump the process table:
	

	while(true)
	{
		//printf("2000");
		graphics_update_fb();
		/*printf("year: %d\n", (uint32_t)rtc_get_year());
		printf("month: %d\n", (uint32_t)rtc_get_month());
		printf("day: %d\n", (uint32_t)rtc_get_day());
		printf("weekday: %d\n", (uint32_t)rtc_get_weekday());
		printf("hour: %d\n", (uint32_t)rtc_get_hour());
		printf("minute: %d\n", (uint32_t)rtc_get_minute());
		printf("second: %d\n", (uint32_t)rtc_get_second());*/
	}
}

void thread_2()
{
	while(true)
	{
		printf("hi");
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
