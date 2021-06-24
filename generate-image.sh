#!/bin/bash

if [ "$1" == "" ]; then
    echo "Usage: $(basename $0) ANGLE"
    exit 1
fi

readonly angle="$1"
readonly angleNNN=$(printf "%03d" $angle)
readonly pfmfile=image$angleNNN.pfm
readonly pngfile=image$angleNNN.png

time .././image-renderer demo --angleDeg=$angle -o $pfmfile \
    && .././image-renderer pfm2ldr png $pfmfile $pngfile
