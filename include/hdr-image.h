#ifndef HDR_IMAGE_H
#define HDR_IMAGE_H

#include <fstream>
#include <vector>
#include "color.h"

struct HdrImage {
	int width, height;
	std::vector<Color> pixels;

	HdrImage(const int width, const int height) {
		this->width = width;
		this->height = height;
	}

	bool validCoordinates(const int x, const int y) {
		return x >= 0 and x < width and y >= 0 and y < height;
	}

	int pixelOffset(int x, int y) {
		return 0;
	}

	Color getPixel(int x, int y) {
		return Color{0.f, 0.f, 0.f};
	}

	void setPixel(int x, int y, Color c) {
	}
	
	void savePfm(std::ofstream &stream, float value) {
	}
};

#endif // HDR_IMAGE_H
