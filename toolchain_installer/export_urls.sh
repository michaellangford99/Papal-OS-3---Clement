GMP_VERSION=$(cat "GMP_VERSION.com")
MPFR_VERSION=$(cat "MPFR_VERSION.com")
MPC_VERSION=$(cat "MPC_VERSION.com")
TEXINFO_VERSION=$(cat "TEXINFO_VERSION.com")

GMP_URL="https://gmplib.org/download/gmp/gmp-$GMP_VERSION.tar.bz2"
MPFR_URL="http://www.mpfr.org/mpfr-current/mpfr-$MPFR_VERSION.tar.bz2"
MPC_URL="ftp://ftp.gnu.org/gnu/mpc/mpc-$MPC_VERSION.tar.gz"
TEXINFO_URL="http://ftp.gnu.org/gnu/texinfo/texinfo-$TEXINFO_VERSION.tar.gz"

echo $GMP_URL
echo $MPFR_URL
echo $MPC_URL
echo $TEXINFO_URL

echo $GMP_URL >> GMP_URL.url
echo $MPFR_URL >> MPFR_URL.url
echo $MPC_URL >> MPC_URL.url
echo $TEXINFO_URL >> TEXINFO_URL.url
