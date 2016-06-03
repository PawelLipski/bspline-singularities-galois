
opts="draw-neighbors draw-plain draw-supports galois gnuplot"
depths="`seq 1 5`"

for opt in $opts; do
	echo "./generate --$opt #${opt}_no-depth"
	for depth in $depths; do
		echo "./generate --$opt $depth #${opt}_depth-${depth}"
	done
done

