#!/bin/bash

DIR="animation_demo/"

cd "build/"
make
if [ ! -d "$DIR" ]
then 
	mkdir $DIR
fi
./image-renderer animation
cd $DIR
echo "I'm almost done..."
for f in *.pfm; do .././image-renderer pfm2ldr png ${f%.pfm}.pfm ${f%.pfm}.png; done
ffmpeg -r 30 -f image2 -s 300x200 -i demo-%03d.png -vcodec libx264 -pix_fmt yuv420p spheres-perspective.mp4
rm -r *.pfm
rm -r *.png
echo "Finished."
echo "Check in your build/$DIR the animation!"