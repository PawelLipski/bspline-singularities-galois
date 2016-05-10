rm -rf eps/
mkdir -p eps gif
for i in `seq 0 5 355`; do
	echo $i
	./render-non-rect-support non-rect-support-`printf "%.3i" $i` $i | gnuplot
done
convert +antialias -delay 10 -density 300 -loop 0 -resize 800x600 eps/non-rect-support-*.eps gif/non-rect-support.gif
