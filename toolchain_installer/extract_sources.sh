for file in `ls *.tar*`
do
        echo $file
	dtrx $file
done
