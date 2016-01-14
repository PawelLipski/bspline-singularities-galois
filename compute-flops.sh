#!/usr/bin/env bash

#usage, for depth=25: ./compute-flops.sh 25

rm -rf test-meshes
mkdir test-meshes
cd test-meshes

MAX_DEPTH=$1

depths="`seq 2 ${MAX_DEPTH}`"

for depth in $depths; do
    ../generate -g --edged-4 ${depth} > ./edged-4-depth-${depth}
done

#make sure you have Konrad Yopek analyser in PATH, https://bitbucket.org/_kjopek/meshestimator

for depth in $depths; do
    analyser -f ./edged-4-depth-${depth} > ./edged-4-depth-${depth}-flops
done

rm -rf edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}*

for depth in $depths; do
    analyser -f ./edged-4-depth-${depth} > ./edged-4-depth-${depth}-flops
done

for depth in $depths; do
    cat edged-4-depth-${depth}-flops | awk "{ sum+=\$3} END {print \"${depth} \" sum}" >> edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum
    cat edged-4-depth-${depth}-flops | awk "BEGIN {max=0} {if (\$1>max) max=\$1} END {print max}" >> edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max
done

paste edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum | awk '{print $1,$3}' > edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum

gnuplot -e "set terminal png; set output 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum.png'; plot 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum' using 1:2 w l"

eog edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_depth_sum.png

gnuplot -e "set terminal png; set output 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum.png'; plot 'edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum' using 1:2 w l"

eog edged-4-flops-per-dept-from-2-to-${MAX_DEPTH}_max_sum.png
