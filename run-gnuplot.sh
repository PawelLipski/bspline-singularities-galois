#!/bin/bash

make && ./tree --gnuplot 3 | ./gnuplot -s $1 | gnuplot
