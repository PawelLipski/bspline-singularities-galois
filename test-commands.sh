
opts="draw-neighbors draw-plain draw-supports galois gnuplot"
meshes="unedged edged-4 edged-8"
depths="`seq 1 5`"

for opt in $opts; do
	for mesh in $meshes; do
		if { [ $opt = "draw-supports" ] || [ $opt = "galois" ]; } && [ $mesh = "unedged" ]; then continue; fi
		echo "./generate --$opt --$mesh #${opt}_no-depth_$mesh"
		for depth in $depths; do
			echo "./generate --$opt --$mesh $depth #${opt}_depth-${depth}_$mesh"
		done
	done
done

for mesh in $meshes; do
	for depth in $depths; do
		echo "./generate --gnuplot --$mesh $depth | ./gnuplot -s #gnuplot_gnuplot_depth-${depth}_$mesh"
	done
done

