TEXINFO_VERSION=$(cat "TEXINFO_VERSION.com")
TEXINFO_DIR="texinfo-$TEXINFO_VERSION"
echo $TEXINFO_DIR

cd $TEXINFO_DIR
./configure