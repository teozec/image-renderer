#!/bin/sh

for i in $(seq 10 50); do
	echo $(expr $i - 9) / 41;
	./image-renderer render --float="angle:-$i,red:0.$(expr \( $i - 10 \) \* 10 / 41),blue:0.$(expr 9 - \( $i - 19 \) \* 10 / 41)" --nRays=3 --depth=2 ../examples/scene.txt --outfile=scene-$i.pfm
	./image-renderer pfm2ldr scene-$i.pfm
done
ffmpeg -r 25 -f image2 -start_number 10 -i scene-%02d.png -vcodec libx264 -pix_fmt yuv420p animation.mp4
