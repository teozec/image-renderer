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
#undef NDEBUG
#include <cassert>
#include "color.h"

enum class Endianness { littleEndian, bigEndian };

struct HdrImage {
	int width, height;
	std::vector<Color> pixels;

	// Construct
	HdrImage(const int width, const int height) : width(width), height(height) {
		pixels.reserve(width * height);
	}

	// Construct from stream
	HdrImage(std::istream &stream){
		readPfmFile(stream);
	}

	// Construct from PFM file
	HdrImage(const std::string &fileName){
		std::istringstream stream(fileName);
		readPfmFile(stream);
	}

	bool validCoordinates(const int x, const int y) {
		return x >= 0 and x < width and y >= 0 and y < height;
	}

	int pixelOffset(const int x, const int y) {
		return x*height + y;
	}

	Color getPixel(const int x, const int y) {
		assert(validCoordinates(x, y));
		return pixels[pixelOffset(x, y)];
	}

	void setPixel(const int x, const int y, const Color c) {
		assert(validCoordinates(x, y));
		pixels[pixelOffset(x, y)] = c;
	}

	void savePfm(std::ostream &stream);
	void readPfmFile(std::istream &stream);
};

class InvalidPfmFileFormat : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

void parseImageSize(std::string line, int &width, int &height);
Endianness parseEndianness(std::string line);

#endif // HDR_IMAGE_H
