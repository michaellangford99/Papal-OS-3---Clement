cd Source
echo "creating c file $1"

#if necessary, create directory
if [[ $1 == */* ]] ; then
	DIR=${1%/*}
	echo $DIR 
	mkdir $DIR
fi

#add new c file to headers list
echo -n '#include "' >> headers.h
echo -n $1 >> headers.h
echo '.h"' >> headers.h

#create c file
CFILE="${1}.c"
if [[ $CFILE == */* ]] ; then
        echo '#include "../system.h"' >> $CFILE
else
	echo '#include "system.h"' >> $CFILE
fi
#create h file
HFILE="${1}.h"
HIF=`echo "$1" | tr '[:lower:]' '[:upper:]'`
HIF="${HIF#*/}"
HIF="${HIF}_H"
echo "#ifndef $HIF" >> $HFILE
echo "#define $HIF" >> $HFILE
echo "" >> $HFILE
echo "#endif" >> $HFILE

cd ..
