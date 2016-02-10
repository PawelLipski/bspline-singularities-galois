
refdir=test-ref-output

save_ref_output() {
	IFS=$'\n'
	mkdir -p $refdir/old
	mv $refdir/*.out $refdir/old
	for line in `./test-commands.sh`; do
		cmd=`cut -d'#' -f1 <<< $line`
		name=`cut -d'#' -f2 <<< $line`
		echo Saving reference output for ${name}...
		eval $cmd > $refdir/${name}.out
	done
}

[ "$1" == "--save-ref-output" ] && { save_ref_output; exit; }

IFS=$'\n'
all_ok=true
for line in `./test-commands.sh`; do
	cmd=`cut -d'#' -f1 <<< $line`
	name=`cut -d'#' -f2 <<< $line`
	eval $cmd > ${name}.out
	cmp ${name}.out $refdir/${name}.out || { echo Error in ${name}.out!; all_ok=false; }
	rm -f ${name}.out
done

$all_ok && echo All ok.

