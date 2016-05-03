#!/bin/bash


edged_4() {
	./generate --draw-plain --edged-4 3 > grid.dat

	cat grid.dat | ./render-bsplines edged-4_overview `seq 0 5` | gnuplot

	for i in `seq 0 6`; do
	#for i in `seq 0 0`; do
		cat grid.dat | ./render-bsplines edged-4_$i $i | gnuplot
	done
}

gnomon() {
	./render-non-rect-support gnomon | gnuplot
}

bspline_sum() {
	for depth in {1..5}; do
		./generate --knots --unedged $depth | ./render-bspline-sum bspline-sum-$depth | gnuplot
	done
}


make || exit 1
mkdir -p png eps

edged_4
gnomon
bspline_sum

rm -rf *.dat gpl.out

for eps in eps/*.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1920x1080 -flatten $png
    fi
done

rm -rf eps/

eog png/gnomon.png

