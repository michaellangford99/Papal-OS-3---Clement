GCC_VERSION="6.3.0"
GCC_URL="ftp://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.bz2"

wget $GCC_URL | tee gcc_log.log

dtrx "gcc-$GCC_VERSION.tar.bz2"
