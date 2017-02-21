cd  Source
export LINES=`( find . \( -name '*.h' -o -name '*.c' \) -print0 | xargs -0 cat ) | wc -l`
echo $LINES
cd ..
