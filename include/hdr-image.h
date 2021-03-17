#ifndef HDR_IMAGE_H
#define HDR_IMAGE_H

#include <fstream>
#include <vector>
#include "color.h"

struct HdrImage {
	int width, height;
	std::vector<Color> pixels;

	bool validCoordinates(int x, int y) {
		return true;
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
