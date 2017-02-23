GMP_VERSION=$(cat "GMP_VERSION.com")
GMP_DIR="gmp-$GMP_VERSION"
echo $GMP_DIR

cd $GMP_DIR
./configure
