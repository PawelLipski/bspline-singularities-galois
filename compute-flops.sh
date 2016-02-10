#!/usr/bin/env bash

# usage, for depth=25: ./compute-flops.sh 25

dir=flops-per-depth
rm -rf $dir
mkdir $dir
cd $dir

MAX_DEPTH=$1
depths="`seq 2 ${MAX_DEPTH}`"

for depth in $depths; do
    ../generate -g --edged-4 $depth > mesh-$depth
done

# make sure you have Konrad Jopek's analyser in PATH, https://bitbucket.org/_kjopek/meshestimator

for depth in $depths; do
    analyser -f mesh-$depth > flops-$depth
done

for depth in $depths; do
    cat flops-$depth | awk "BEGIN { max=0 } { sum+=\$3; if (\$1>max) max=\$1 } END {print max, sum}" >> total-flops
done

#gnuplot -e "set terminal png; set output 'flops-per-depth-from-2-to-${MAX_DEPTH}_max_sum.png'; g(x) = x**3; f(x) = a*x**b; b = 2; a = 2; fit f(x) 'edged-4-flops-per-depth-from-2-to-${MAX_DEPTH}_max_sum' via a, b; plot 'edged-4-flops-per-depth-from-2-to-${MAX_DEPTH}_max_sum' w l, f(x)"
mkdir -p ../flops-plots
gnuplot -e "set terminal png; set output '../flops-plots/total-flops.png'; fit(x) = x**b; b = 2; fit fit(x) 'total-flops' via b; set xlabel 'N'; set ylabel 'Flops'; plot 'total-flops' w l title 'measured flops(N)', fit(x) title sprintf('x ^ %g', b) w l, x ** 2 title 'x ^ 2' w l"

eog ../flops-plots/total-flops.png

#rm -rf $dir

