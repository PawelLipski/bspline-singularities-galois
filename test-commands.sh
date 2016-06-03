
opts="draw-neighbors draw-plain draw-supports galois gnuplot"
shapes="q r"
depths="`seq 1 5`"

for opt in $opts; do
	for shape in $shapes; do
		echo "./generate --$opt -$shape #${opt}_no-depth_$shape"
		for depth in $depths; do
			echo "./generate --$opt -$shape $depth #${opt}_depth-${depth}_$shape"
		done
	done
done

