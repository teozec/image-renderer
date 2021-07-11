#!/bin/sh

./image-renderer --antialiasing=4 --depth=3 --roulette=2 demo
./image-renderer pfm2ldr demo.pfm
