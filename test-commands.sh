
opts="draw-neighbors draw-plain draw-supports galois gnuplot"
meshes="edged-4"
depths="`seq 1 5`"

for opt in $opts; do
	for mesh in $meshes; do
		echo "./generate --$opt --$mesh #${opt}_no-depth_$mesh"
		for depth in $depths; do
			echo "./generate --$opt --$mesh $depth #${opt}_depth-${depth}_$mesh"
		done
	done
done

