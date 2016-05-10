rm -rf eps/
mkdir -p eps gif
for i in `seq 0 5 355`; do
	echo $i
	./render-non-rect-support non-rect-support-`printf "%.3i" $i` $i | gnuplot
done
convert -delay 10 -density 150 -loop 0 -resize 1024x768 eps/non-rect-support-*.eps gif/non-rect-support.gif
