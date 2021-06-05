#!/bin/bash

if [ "$1" == "" ]; then
    echo "Usage: $(basename $0) ANGLE"
    exit 1
fi

readonly angle="$1"
readonly angleNNN=$(printf "%03d" $angle)
readonly pfmfile=image$angleNNN.pfm
readonly pngfile=image$angleNNN.png

time build/image-renderer demo --angleDeg=$angle \
    -w 1280 -h 960 -o $pfmfile \
    && build/image-renderer pfm2ldr png $pfmfile $pngfile
