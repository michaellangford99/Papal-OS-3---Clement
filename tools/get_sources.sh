./versions.sh
./export_urls.sh

GMP_URL=$(cat "GMP_URL.url")
MPFR_URL=$(cat "MPFR_URL.url")
MPC_URL=$(cat "MPC_URL.url")
TEXINFO_URL=$(cat "TEXINFO_URL.url")

echo $GMP_URL
echo $MPFR_URL
echo $MPC_URL
echo $TEXINFO_URL

#mkdir -p GMP
#mkdir -p MPFR
#mkdir -p MPC
#mkdir -p TEXINFO

wget "$GMP_URL" | tee gmp.log
wget "$MPFR_URL" | tee mpfr.log
wget "$MPC_URL" | tee mpc.log
wget "$TEXINFO_URL" | tee texinfo.log

echo
echo "Done!"

./extract_sources.sh
