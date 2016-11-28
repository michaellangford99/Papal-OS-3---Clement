# written by Michael Langford, based off tutorials "Bare Bones"
# and "Setting up a Cross Compiler" in osdev.org
clear

#prep cross-compiler
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export PATH="$HOME/opt/cross/bin:$PATH"
#settings
Ccolor=220
LINKcolor=21
GRUBcolor=121

CFLAGS="-std=gnu99 -ffreestanding -O2 -Wextra -Wall -fdiagnostics-color=always"
LD_BIN_FLAGS="-T Source/linker.ld -o Bin/clement.bin -ffreestanding -O2 -nostdlib"
LD_O_FLAGS="-lgcc -fdiagnostics-color=always"

LOGFILE="buildlog.log"

#assemble special assembly files
i686-elf-as Source/boot.s            -o Bin/boot.o
nasm -felf  Source/cpu/gdt.asm       -o Bin/gdts.o
nasm -felf  Source/cpu/idt.asm       -o Bin/idts.o
i686-elf-as Source/mem/paging.s      -o Bin/pagings.o

#compile c files
cd Source
#find all directories in Source dir
SRC_DIRS=`ls -d */ | sed 's#/##'`

#compile all c files in Source dir
for cfile in `ls *.c`
do
	i686-elf-gcc -c  $cfile -o "../Bin/$cfile.o" $CFLAGS
	tput setaf $Ccolor
	echo $cfile
	tput sgr0
done

#compile all c files in Source subdirectories
for dir in $SRC_DIRS
do
        cd $dir
	for cfile in `ls *.c`
	do
		i686-elf-gcc -c  $cfile -o "../../Bin/$cfile.o" $CFLAGS
		tput setaf $Ccolor
		echo $cfile
		tput sgr0
	done
	cd ..
done
cd ..

#done with c files

#link
O_FILES=`ls Bin/*.o`
tput setaf $LINKcolor
echo $O_FILES
tput sgr0
i686-elf-gcc $LD_BIN_FLAGS $O_FILES $LD_O_FLAGS

#prepare GRUB
cp Bin/clement.bin Grub/boot/clement.bin
cp Grub/grub.cfg Grub/boot/grub/grub.cfg

#create .iso
tput setaf $GRUBcolor
grub-mkrescue -o Build/clement.iso Grub

#we're done!
echo "--Build Clement OS--" >> $LOGFILE
date >> $LOGFILE
echo "" >> $LOGFILE

echo "Finished!"
tput sgr0
