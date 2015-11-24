
for opt in draw-neighbors draw-supports galois gnuplot; do
	for mesh in new-mesh old-mesh; do
		echo "./tree --$opt --$mesh #${opt}_no-depth_$mesh"
		for depth in `seq 1 5`; do
			echo "./tree --$opt --$mesh $depth #${opt}_depth-${depth}_$mesh"
		done
	done
done

for mesh in new-mesh old-mesh; do
	for depth in `seq 1 5`; do
		echo "./tree --gnuplot --$mesh $depth | ./gnuplot -s #gnuplot_gnuplot_depth-${depth}_$mesh"
	done
done

