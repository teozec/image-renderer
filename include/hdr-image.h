#ifndef HDR_IMAGE_H
#define HDR_IMAGE_H

#include <fstream>
#include <vector>
#undef NDEBUG
#include <cassert>
#include "color.h"

struct HdrImage {
	const int width, height;
	std::vector<Color> pixels;

	// Construct
	HdrImage(const int width, const int height) : width(width), height(height) {
		pixels.reserve(width * height);
	}

	// Construct from stream
	HdrImage(std::istream &stream) {
		readPfmFile(stream);
	}

	// Construct from PFM file
	HdrImage(const std::string &fileName){
		std::ifstream stream{fileName};
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

    void savePfm(std::stringstream &stream);
    void readPfmFile(std::istream &stream);
};

#endif // HDR_IMAGE_H
