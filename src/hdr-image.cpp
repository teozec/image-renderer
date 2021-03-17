#include "hdr-image.h"

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
