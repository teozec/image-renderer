#!/bin/bash

./image-renderer --nRays=2 --depth=3 demo
./image-renderer pfm2ldr demo.pfm
