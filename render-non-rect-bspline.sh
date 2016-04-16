#!/bin/bash

make || exit 1
mkdir -p png eps

./non-rect-support gnomon | gnuplot

rm -rf *.dat gpl.out

for eps in eps/gnomon.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1920x1080 -flatten $png
    fi
done

rm -rf eps

