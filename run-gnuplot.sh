#!/bin/bash

make || exit 1
./tree --gnuplot 2 > grid.dat
mkdir -p png eps

./gnuplot bspline-012 0 1 2 < grid.dat | gnuplot
./gnuplot bspline-345 3 4 5 < grid.dat | gnuplot
for i in `seq 0 5`; do
	./gnuplot bspline-$i $i < grid.dat | gnuplot
done
rm -rf bspline*.dat grid.dat gpl.out

for eps in eps/*.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1024x768 -flatten $png
    fi
done

