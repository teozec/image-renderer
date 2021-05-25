#!/bin/bash

DIR="animation_demo/"

cd "build/"
make
if [ ! -d "$DIR" ]
then 
	mkdir $DIR
fi
cd $DIR
parallel -j 4 ../.././generate-image.sh '{}' ::: $(seq 0 359)

ffmpeg -r 60 -f image2 -s 1280x960 -i image%03d.png -vcodec libx264 -pix_fmt yuv420p animation-perspective.mp4
rm -r *.pfm
rm -r *.png
echo -ne '\n'
echo "Finished."
echo "Check in your 'image-renderer/build/$DIR' the animation!"