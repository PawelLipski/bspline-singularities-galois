#!/bin/bash

make || exit 1
./tree --gnuplot 2 > grid.dat
mkdir -p png eps
i=0
while :; do
	./gnuplot $[i++] < grid.dat > gpl.out 2>/dev/null || break
	gnuplot gpl.out
done
rm -rf bspline.dat grid.dat gpl.out

for eps in eps/*.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1024x768 -flatten $png
    fi
done

