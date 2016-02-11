
Ne=${1-10}
Nb=$[Ne+1]
echo $Nb
seq $Nb | xargs -I@ echo @ 1
echo $Ne
seq $Ne | awk '{print 1, $1, 2, $1, $1 + 1}'
Nt=$[Ne*2-1]
echo $Nt
for i in `seq $[Ne-1]`; do
	echo -n $[i*2-1] $[Ne-i+1] '  '
	seq $i $Ne | xargs -I@ echo -n 1 @ ' '
	echo '' $[i*2] $[i*2+1]
	echo $[i*2] 1 ' ' 1 $i
done
echo $Nt 1 ' ' 1 $Ne
