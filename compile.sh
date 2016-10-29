# written by Michael Langford, based off tutorials "Bare Bones"
# and "Setting up a Cross Compiler" in osdev.org
clear

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export PATH="$HOME/opt/cross/bin:$PATH"

i686-elf-as Source/boot.s            -o Bin/boot.o
nasm -felf  Source/cpu/gdt.asm       -o Bin/gdts.o
nasm -felf  Source/cpu/idt.asm       -o Bin/idts.o
i686-elf-as Source/mem/paging.s      -o Bin/pagings.o

i686-elf-gcc -c Source/kernel.c         -o Bin/kernel.o          -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/multiboot.c      -o Bin/multiboot.o       -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/mem/mem.c        -o Bin/mem.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/mem/paging.c     -o Bin/paging.o          -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/tools/bintools.c -o Bin/bintools.o        -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/console.c        -o Bin/console.o         -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/cpu/io.c         -o Bin/io.o              -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/keyboard.c       -o Bin/keyboard.o        -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/vga.c            -o Bin/vga.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/cpu/interrupt.c  -o Bin/interrupt.o       -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/cpu/pic.c        -o Bin/pic.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/string.c         -o Bin/string.o          -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/math.c           -o Bin/math.o            -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/cpu/gdt.c        -o Bin/gdt.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/cpu/idt.c        -o Bin/idt.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/clock.c          -o Bin/clock.o           -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/ata.c            -o Bin/ata.o             -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/FS/fs.c          -o Bin/fs.o              -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/FS/clement_vfs.c -o Bin/clement_vfs.o     -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c Source/FS/fat16.c       -o Bin/fat16.o           -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i686-elf-gcc -T Source/linker.ld -o Bin/clement.bin -ffreestanding -O2 -nostdlib Bin/boot.o Bin/io.o Bin/mem.o Bin/paging.o Bin/pagings.o Bin/multiboot.o Bin/kernel.o Bin/console.o Bin/keyboard.o Bin/ata.o Bin/vga.o Bin/interrupt.o Bin/pic.o Bin/string.o Bin/math.o Bin/gdt.o Bin/gdts.o Bin/idt.o Bin/idts.o Bin/clock.o Bin/fs.o Bin/clement_vfs.o Bin/fat16.o Bin/bintools.o -lgcc

cp Bin/clement.bin Grub/boot/clement.bin
cp Grub/grub.cfg Grub/boot/grub/grub.cfg

grub-mkrescue -o Build/clement.iso Grub

echo "--Build Clement OS--" >> buildlog.log
date >> buildlog.log
echo "" >> buildlog.log

echo "Finished!"
