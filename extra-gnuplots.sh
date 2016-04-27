#!/bin/bash

make || exit 1
mkdir -p png eps

./render-non-rect-support gnomon | gnuplot
for depth in {1..5}; do
	./generate --knots --unedged $depth | ./render-bspline-sum bspline-sum-$depth | gnuplot
done

rm -rf *.dat gpl.out

for eps in eps/*; do
	png=`sed 's/eps/png/g' <<< "$eps"`
	echo $eps '->' $png
	convert +antialias -density 300 $eps -resize 1920x1080 -flatten $png
done

rm -rf eps
eog png/gnomon.png
