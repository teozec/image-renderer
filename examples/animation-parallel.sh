#!/bin/sh

CORES=${1:-5}

parallel -k --lb -j $CORES 'sh -c "echo $(expr {} - 9) / 41; ./image-renderer render --float=\"angle:-{},red:0.$(expr \( {} - 10 \) \* 10 / 41),blue:0.$(expr 9 - \( {} - 19 \) \* 10 / 41)\" --nRays=3 --depth=2 ../examples/scene.txt --outfile=scene-{}.pfm"' ::: $(seq 10 50)
parallel -j 4 './image-renderer pfm2ldr scene-{}.pfm' ::: $(seq 10 50)
ffmpeg -r 25 -f image2 -start_number 10 -i scene-%02d.png -vcodec libx264 -pix_fmt yuv420p animation.mp4
