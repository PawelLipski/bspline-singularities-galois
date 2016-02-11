#!/bin/bash

# usage, for depth=25: ./compute-flops.sh 25

rm -rf tmp
mkdir tmp

max_depth=${1-25}
generate_cmd=${2-../generate -g -4}
output_prefix=${3-flops-generate}

for depth in `seq $max_depth`; do
    $generate_cmd $depth > tmp/mesh-$depth
    ~/meshestimator/analyser -f tmp/mesh-$depth > tmp/flops-$depth
    cat tmp/flops-$depth | awk 'BEGIN { max=0 } { sum+=$3; if ($1>max) max=$1 } END {print max+1, sum}' >> tmp/total-flops
done

x__b() {
	output_suffix=x__b
	fit_fun='x**b'
	fit_init='b = 2'
	fit_via='b'
	fit_fun_sprintf="'x ^ %g', b"
}

a_x__b() {
	output_suffix=a_x__b
	fit_fun='a*x**b'
	fit_init='a = 2; b = 2'
	fit_via='a, b'
	fit_fun_sprintf="'%g * N ^ %g', a, b"
}

plot() {
gnuplot << EOF
	set terminal png
	set output '${output_prefix}-${output_suffix}.png'
	fit(x) = $fit_fun
	$fit_init
	fit fit(x) 'tmp/total-flops' via $fit_via
	set xlabel 'N'
	set ylabel 'Flops'
	plot 'tmp/total-flops' w l title 'measured flops(N)', \
		fit(x) title sprintf($fit_fun_sprintf) w l, \
		x ** 2 title 'N ^ 2' w l
EOF
}

x__b
plot
a_x__b
plot

eog ${output_prefix}-*.png

