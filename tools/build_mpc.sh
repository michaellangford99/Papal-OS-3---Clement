MPC_VERSION=$(cat "MPC_VERSION.com")
MPC_DIR="mpc-$MPC_VERSION"
echo $MPC_DIR

cd $MPC_DIR
./configure
