#!/bin/bash

make || exit 1
mkdir -p png eps

./tree --gnuplot --edged-8 3 > edged-8_grid.dat
./gnuplot edged-8_overview 0 2 3 6 7 12 < edged-8_grid.dat | gnuplot
for i in `seq 0 12`; do
	./gnuplot edged-8_$i $i < edged-8_grid.dat | gnuplot
done

./tree --gnuplot --unedged 4 > unedged_grid.dat
./gnuplot unedged_overview 13 14 < unedged_grid.dat | gnuplot

rm -rf *.dat gpl.out

for eps in eps/*.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1920x1080 -flatten $png
    fi
done

