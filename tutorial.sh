#!/bin/bash

DIR="animation_demo/"

cd "build/"
make
if [ ! -d "$DIR" ]
then 
	mkdir $DIR
fi
cd $DIR
for angle in $(seq 0 359); do
    angleNNN=$(printf "%03d" $angle)
    .././image-renderer demo -w 320 -h 180 --angleDeg=$angle -o "img$angleNNN.pfm"
done
echo "I'm almost done..."
for f in *.pfm; do .././image-renderer pfm2ldr png ${f%.pfm}.pfm ${f%.pfm}.png; done
ffmpeg -r 30 -f image2 -s 320x180 -i img%03d.png -vcodec libx264 -pix_fmt yuv420p spheres-perspective.mp4
rm -r *.pfm
rm -r *.png
echo "Finished."
echo "Check in your build/$DIR the animation!"