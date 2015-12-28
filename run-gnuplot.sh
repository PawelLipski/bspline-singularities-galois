#!/bin/bash

make && ./generate --gnuplot 3 | ./gnuplot -s $1 | gnuplot
rm -f bspline${1}.dat

