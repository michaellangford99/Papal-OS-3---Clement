echo "creating c file $1"

echo -n '#include "' >> Source/headers.h
echo -n $1 >> Source/headers.h
echo '.h"' >> Source/headers.h
