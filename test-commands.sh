
for opt in draw-neighbors draw-supports galois gnuplot; do
	echo "./tree --$opt #${opt}_no-depth"
	for depth in `seq 1 5`; do
		echo "./tree --$opt $depth #${opt}_depth-$depth"
	done
done

for depth in `seq 1 5`; do
	echo "./tree --gnuplot $depth | ./gnuplot -s #gnuplot_gnuplot_depth-$depth"
done
