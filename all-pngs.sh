#!/bin/bash


nurbs() {
	for i in 1 9 18 21 36 41 44; do
		./render-bsplines nurbs-$i 45 Regular $i | gnuplot
	done
}

gnomon() {
	./render-non-rect-support gnomon-trunk 330 1 | gnuplot
	./render-non-rect-support gnomon-xshifted 330 2 | gnuplot
	./render-non-rect-support gnomon-yshifted 330 3 | gnuplot
	./render-non-rect-support gnomon-glue 330 4 | gnuplot
}

bspline_sum() {
	for depth in {1..5}; do
		echo Sum for depth = ${depth}...
		./generate --knots --edged-4 $depth | ./render-bspline-sum bspline-sum-$depth | gnuplot
	done
}


make || exit 1
rm -rf eps/
mkdir -p png eps

#nurbs
gnomon
#bspline_sum

rm -rf *.dat gpl.out

for eps in eps/*.eps; do
    if [ -f "$eps" ]; then
        png=`sed 's/eps/png/g' <<< "$eps"`
        echo $eps '->' $png
		convert +antialias -density 300 $eps -resize 1920x1080 -flatten $png
    fi
done

rm -rf eps/

#eog png/gnomon.png

