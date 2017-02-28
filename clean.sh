rm Bin/*.o
rm Bin/*.bin
rm Build/*.iso
rm Grub/boot/clement.bin


cd sysroot/usr/include

SRC_DIRS=`ls -d */ | sed 's#/##'`
SRC_DIRS=". $SRC_DIRS"

HFILES=""

#create list of all h files
for dir in $SRC_DIRS
do
	for hfile in `ls $dir/*.h`
	do
		HFILES="$HFILES $hfile"
	done
done

echo $HFILES

for file in $HFILES
do
  echo "removing $PWD/$file"
  rm $file
done



SRC_DIRS=`ls -d */ | sed 's#/##'`
for dir in $SRC_DIRS
do
  echo "removing $PWD/$dir"
  rm -d $dir
done
