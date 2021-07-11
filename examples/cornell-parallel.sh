#!/bin/sh

N=20
CORES=${1:-5}

parallel -k --lb -j $CORES "sh -c \"echo {} / $N; ./image-renderer render --width=600 --height=600 --seed={} --antialiasing=9 --nMax=4 --depth=3 --roulette=3 --outfile=cornell-{}.pfm ../examples/cornell.txt\"" ::: $(seq 1 $N)
./image-renderer stack --method=mean cornell-*.pfm --outfile=cornell.pfm
./image-renderer pfm2ldr cornell.pfm
