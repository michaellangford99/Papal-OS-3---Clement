/*
Copyright (C) 2016 Michael Langford
This software is distributed under the GNU General Public License.

This kernel is based off of the "Bare Bones" demo on osdev.org
*/

#include "system.h"

void kernel_main(struct multiboot_header* mboot_header, uint32_t multiboot_magic) {
	
	int continue_boot = multiboot_init(mboot_header, multiboot_magic);
	if (continue_boot == K_FATAL) { /*loop forever, don't boot*/ while(true) { } }
	
	memory_init(multiboot_get_address());              //. gotta be first
	
	graphics_init(multiboot_get_vbe_address());        //basic initial boot-time drivers
	
	console_init();                                    //.
	
	//start basic drivers
	gdt_init();                                        //.
	idt_init();                                        //.
	clock_init();                                      //.
	keyboard_init();                                   //.
	
	//graphics_test();
	
	//start higher level drivers
	ata_init();                                        //.
	fs_init();                                         //.
	
	//data dump
	multiboot_dump(multiboot_get_address());           //.
	
	
	fs_format(1, FS_CLEMENT_VFS);
	clement_vfs_mkfile("file1", get_device(1));
	clement_vfs_write("file1", get_device(1), FILE_OVERWRITE, "hi, this is a very stupid test\nI hope you like it! Bye!");	
	clement_vfs_mkfile("file2", get_device(1));
	clement_vfs_write("file2", get_device(1), FILE_OVERWRITE, "hi again!\n\n\n\n\n\n\nlots of eols!\n");
	clement_vfs_mkfile("file3", get_device(1));
	clement_vfs_write("file3", get_device(1), FILE_OVERWRITE, "hi again!\n\n\n\n\n\nmore of eols!\n");
	
	clement_vfs_delfile("file2", get_device(1));
	
	printf("Clement 2.0 @ VirtualBox Ubuntu >> ");
	
	
	while(true)
	{
		
	}
}
