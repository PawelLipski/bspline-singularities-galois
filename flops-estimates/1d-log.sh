
node_id=0

tree() {
	: $[node_id++]
	args=("$@")
	begin_len=$[$#/2]
	end_len=$[$#-begin_len]
	local begin=${args[@]:0:$begin_len}
	local end=${args[@]:$begin_len:$end_len}
	#echo $# / $@ / $begin_len / $end_len / $begin / $end

	echo -n $[node_id] $# '' $(xargs -d' ' -I% echo 1 % <<< $@) ' '
	[ $# -gt 1 ] && {
		left=$[node_id+1]
		right=$[node_id+2*begin_len]
		echo $left $right
		tree $begin
		tree $end
	} || echo
}

Ne=${1-10}
Nb=$[Ne+1]
echo $Nb
seq $Nb | xargs -I@ echo @ 1
echo $Ne
seq $Ne | awk '{print 1, $1, 2, $1, $1 + 1}'
Nt=$[Ne*2-1]
echo $Nt
tree `seq $Ne`

