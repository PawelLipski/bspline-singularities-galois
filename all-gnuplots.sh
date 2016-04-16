#!/bin/bash

make || exit 1
mkdir -p png eps

#./generate --gnuplot --edged-8 3 > edged-8_grid.dat
./generate --gnuplot --edged-4 3 > edged-4_grid.dat
#./render-bsplines edged-8_overview 0 2 3 6 7 12 < edged-8_grid.dat | gnuplot
#for i in `seq 0 12`; do
#	./render-bsplines edged-8_$i $i < edged-8_grid.dat | gnuplot
#done

./generate --draw-plain --edged-4 3 | ./render-bsplines edged-4_overview `seq 0 5` | gnuplot

for i in `seq 0 5`; do
	./render-bsplines edged-4_$i $i < edged-4_grid.dat | gnuplot
done

#./generate --gnuplot --unedged 4 | ./render-bsplines unedged_overview 19 20 | gnuplot

./non-rect-support gnomon | gnuplot

rm -rf *.dat gpl.out

for eps in eps/*.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1920x1080 -flatten $png
    fi
done

rm -rf eps

