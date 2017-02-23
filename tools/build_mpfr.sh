MPFR_VERSION=$(cat "MPFR_VERSION.com")
MPFR_DIR="mpfr-$MPFR_VERSION"
echo $MPFR_DIR

cd $MPFR_DIR
./configure
