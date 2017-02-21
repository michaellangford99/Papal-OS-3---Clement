SRC_DIR="Source"
SYSROOT_DIR="sysroot/usr/include"

HCOLOR=20
tput setaf $HCOLOR

cd $SRC_DIR

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

echo "now copying headers"
cd ..
echo "PWD: $PWD"

#create directory structure
for dir in $SRC_DIRS
do
	echo "creating directiory: $SYSROOT_DIR/$dir"
	mkdir -p $SYSROOT_DIR/$dir
done

#copy the headers
for h in $HFILES
do
	cp $SRC_DIR/$h $SYSROOT_DIR/$h -RT
done

tput sgr0