#!/bin/bash
binary=$1
playroot=$2
testset=$3

tmpdir=$(mktemp -d -p $playroot)

TIMEFORMAT='%3U'
time (
for file in $(cat $playroot/testsets/$testset); do
    grep -E "^[0-9]+ " $playroot/games/$file > $tmpdir/${file}.ref
    $binary < $tmpdir/${file}.ref 2>&1 >/dev/null | grep -E "^[0-9]+ " > $tmpdir/${file}.out
    
    if diff $tmpdir/${file}.ref $tmpdir/${file}.out > /dev/null ; then
	rm $tmpdir/${file}.ref $tmpdir/${file}.out
    else 
	echo "FAIL"
	exit 1
    fi
done
 ) 2> $tmpdir/time

cat $tmpdir/time
rm -Rf $tmpdir
