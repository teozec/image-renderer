#ifndef HDR_IMAGE_H
#define HDR_IMAGE_H

#include <fstream>
#include <vector>
#include "color.h"

struct HdrImage {
	const int width, height;
	std::vector<Color> pixels;

	bool validCoordinates(const int x, const int y) {
		return x >= 0 and x < width and y >= 0 and y < height;
	}

	int pixelOffset(int x, int y) {
		return x*height + y;
	}

	Color getPixel(int x, int y) {
		return Color{0.f, 0.f, 0.f};
	}

	void setPixel(int x, int y, Color c) {
		assert(validCoordinates(x, y));
		pixels[pixelOffset(x, y)] = c;
	}
	
	void savePfm(std::ofstream &stream, float value) {
	}
};

#endif // HDR_IMAGE_H
