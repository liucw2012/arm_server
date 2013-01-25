for i in `seq 1 40`
do
	for j in `seq 3020 3024`
	do
		./Client 10.32.228.76 aa $j &
		#./Client 10.232.97.2 aa $j &
		sleep 1
		echo $j,$i
	done
done
