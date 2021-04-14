/* Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini

This file is part of image-renderer.

image-renderer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

image-renderer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with image-renderer.  If not, see <https://www.gnu.org/licenses/>. */

#ifndef HDR_IMAGE_H
#define HDR_IMAGE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <gd.h>
#undef NDEBUG
#include <cassert>
#include "color.h"

enum class Endianness { littleEndian, bigEndian };

float clamp(const float x);

struct HdrImage {
	int width, height;
	std::vector<Color> pixels;

	// Constructor
	HdrImage() { }
	HdrImage(const int width, const int height) : width(width), height(height) {
		pixels.resize(width * height);
	}

	// Constructor from stream
	HdrImage(std::istream &stream){
		readPfm(stream);
	}

	// Constructor from PFM file
	HdrImage(const std::string &fileName){
		readPfm(fileName);
	}

	// Checks whether or not x and y are valid
	bool validCoordinates(const int x, const int y) {
		return x >= 0 and x < width and y >= 0 and y < height;
	}

	// Evaluate index for pixels[]
	int pixelOffset(const int x, const int y) {
		return x*height + y;
	}

	// Gets color of the pixel in (x, y)
	Color getPixel(const int x, const int y) {
		assert(validCoordinates(x, y));
		return pixels[pixelOffset(x, y)];
	}

	// Sets the color of a pixel
	void setPixel(const int x, const int y, const Color c) {
		assert(validCoordinates(x, y));
		pixels[pixelOffset(x, y)] = c;
	}

	// Evalutate average luminosity with log-average
	float averageLuminosity(float delta=1e-10) {
		float s = 0.0;
		for (auto it = pixels.begin(); it != pixels.end(); ++it)
			s += std::log10(delta + it->luminosity());
		return pow(10, s / pixels.size());
	}

	// write&read pfm file image
	void writePfm(std::ostream &stream, Endianness endianness=Endianness::littleEndian);
	void readPfm(std::istream &stream);
	void readPfm(std::string fileName) {
		std::ifstream stream;
		stream.exceptions(std::ios::failbit | std::ios::badbit);
		stream.open(fileName);
		readPfm(stream);
	}

	// Normalization of pixels given a factor and (optional) luminosity
	void normalizeImage(const float factor, const float luminosity){
		for (auto it = pixels.begin(); it != pixels.end(); ++it){
			(*it) = (*it)*(factor/luminosity);
		}
	}

	void normalizeImage(const float factor){
		float luminosity = averageLuminosity();
		normalizeImage(factor, luminosity);
	}

	// Apply clump function to color components
	void clampImage(){
		for (auto it = pixels.begin(); it != pixels.end(); ++it){
			it->r = clamp(it->r);
			it->g = clamp(it->g);
			it->b = clamp(it->b);
		}
	}

	// Write png image file
	void writePng(const char filename[], int compression, bool palette, float gamma);
	// Write webp image file
	void writeWebp(const char filename[], int quality, float gamma);
	// Write jpeg image file
	void writeJpeg(const char filename[], int quality, float gamma);
	// Write tiff image file
	void writeTiff(const char filename[], float gamma);
	// Write bmp image file
	void writeBmp(const char filename[], float gamma);
	// Write gif image file
	void writeGif(const char filename[], float gamma);
private:
	// Write the image to a gdImagePtr
	gdImagePtr writeGdImage(float gamma);
};

class InvalidPfmFileFormat : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

void parseImageSize(std::string line, int &width, int &height);
Endianness parseEndianness(std::string line);

#endif // HDR_IMAGE_H
