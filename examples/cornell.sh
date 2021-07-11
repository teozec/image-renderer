#!/bin/sh

N=20

for i in $(seq 1 $N); do
	echo $i / $N
	./image-renderer render --width=600 --height=600 --seed=$i --antialiasing=9 --nMax=4 --depth=3 --roulette=3 --outfile=cornell-$i.pfm ../examples/cornell.txt
done
./image-renderer stack --method=mean cornell-*.pfm --outfile=cornell.pfm
./image-renderer pfm2ldr cornell.pfm
