#!/bin/bash
# written by Michael Langford, based off tutorials "Bare Bones"
# and "Setting up a Cross Compiler" in osdev.org
clear
echo "Preparing to compile Clement..."
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export PATH="$HOME/opt/cross/bin:$PATH"
cd
cd 'Operating System/Clement'
clear

i686-elf-as Source/boot.s -o Bin/boot.o
nasm -felf Source/gdt.asm -o Bin/gdts.o
nasm -felf Source/idt.asm -o Bin/idts.o

i686-elf-gcc -c Source/kernel.c         -o Bin/kernel.o          -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/multiboot.c      -o Bin/multiboot.o       -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/mem/mem.c        -o Bin/mem.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/tools/bintools.c -o Bin/bintools.o        -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/console.c        -o Bin/console.o         -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/io.c             -o Bin/io.o              -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/keyboard.c       -o Bin/keyboard.o        -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/vga.c            -o Bin/vga.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/interrupt.c      -o Bin/interrupt.o       -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/pic.c            -o Bin/pic.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/string.c         -o Bin/string.o          -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/math.c           -o Bin/math.o            -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/gdt.c            -o Bin/gdt.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/idt.c            -o Bin/idt.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/clock.c          -o Bin/clock.o           -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/ata.c            -o Bin/ata.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/FS/fs.c          -o Bin/fs.o              -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/FS/clement_vfs.c -o Bin/clement_vfs.o     -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/FS/fat16.c       -o Bin/fat16.o           -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i686-elf-gcc -T Source/linker.ld -o Bin/clement.bin -ffreestanding -O2 -nostdlib Bin/boot.o Bin/io.o Bin/mem.o Bin/multiboot.o Bin/kernel.o Bin/console.o Bin/keyboard.o Bin/ata.o Bin/vga.o Bin/interrupt.o Bin/pic.o Bin/string.o Bin/math.o Bin/gdt.o Bin/gdts.o Bin/idt.o Bin/idts.o Bin/clock.o Bin/fs.o Bin/clement_vfs.o Bin/fat16.o Bin/bintools.o -lgcc

cp Bin/clement.bin Grub_Settings/boot/clement.bin
cp Grub_Settings/grub.cfg Grub_Settings/boot/grub/grub.cfg

grub-mkrescue -o Build/clement.iso Grub_Settings

echo "--Build Clement OS--" >> buildlog.log
date >> buildlog.log
echo "" >> buildlog.log

echo "Finished!"
