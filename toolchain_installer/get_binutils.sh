BINUTILS_VERSION="2.27"
BINUTILS_URL="ftp://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz"

wget $BINUTILS_URL | tee binutils_log.log

dtrx "binutils-$BINUTILS_VERSION.tar.gz"
