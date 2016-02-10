#!/usr/bin/env bash

#usage, for depth=25: ./compute-flops.sh 25

dir=flops-per-depth
rm -rf $dir
mkdir $dir
cd $dir

MAX_DEPTH=$1

depths="`seq 2 ${MAX_DEPTH}`"

for depth in $depths; do
    ../generate -g --edged-4 ${depth} > ./edged-4-depth-${depth}
done

# make sure you have Konrad Jopek's analyser in PATH, https://bitbucket.org/_kjopek/meshestimator

for depth in $depths; do
    analyser -f ./edged-4-depth-${depth} > ./edged-4-depth-${depth}-flops
done

for depth in $depths; do
    cat edged-4-depth-${depth}-flops | awk "{ sum+=\$3} END {print \"${depth} \" sum}" >> edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum
    cat edged-4-depth-${depth}-flops | awk "BEGIN {max=0} {if (\$1>max) max=\$1} END {print max}" >> edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max
done

paste edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum | awk '{print $1,$3}' > edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum

#gnuplot -e "set terminal png; set output 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum.png'; g(x) = x**3; f(x) = a*x**b; b = 2; a = 2; fit f(x) 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum' via a, b; plot 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum' w l, f(x)"
mkdir -p ../flops-plots
gnuplot -e "set terminal png; set output '../flops-plots/edged-4-flops-per-depth-from-2-to-${MAX_DEPTH}_max_sum.png'; fit(x) = x**b; b = 2; fit fit(x) 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum' via b; plot 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum' w l title 'Flops', fit(x) title sprintf('x ^ %g', b) w l, x ** 2 title 'x ^ 2' w l"

eog edged-4-flops-per-depth-from-2-to-${MAX_DEPTH}_max_sum.png

#rm -rf $dir

