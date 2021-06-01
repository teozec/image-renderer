#!/bin/bash

DIR="animation_demo/"

cd "build/"
make
if [ ! -d "$DIR" ]
then 
	mkdir $DIR
fi
cd $DIR
declare -i p
declare -i c=0
declare -i min=0
declare -i max=359
for angle in $(seq $min $max); do
    angleNNN=$(printf "%03d" $angle)
    .././image-renderer demo -w 500 -h 500 -p perspective --angleDeg=$angle -o "img$angleNNN.pfm"
    c=c+1
    (( p=(c+$max-1)*100/$max-100 ))
    echo -ne "[$p%] Raytracing...\r"
done
echo -ne '\n'
declare -i c=0
echo "I'm almost done..."
for f in *.pfm; do
	.././image-renderer pfm2ldr png ${f%.pfm}.pfm ${f%.pfm}.png
    c=c+1
    (( p=(c+$max-1)*100/$max-100 ))
    echo -ne "[$p%] Converting to png...\r"
done
ffmpeg -r 60 -f image2 -s 500x500 -i img%03d.png -vcodec libx264 -pix_fmt yuv420p animation-perspective2.mp4
rm -r *.pfm
rm -r *.png
echo -ne '\n'
echo "Finished."
echo "Check in your 'image-renderer/build/$DIR' the animation!"