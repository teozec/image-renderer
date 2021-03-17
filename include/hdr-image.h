#include <fstream>
#include <vector>
#include "color.h"

struct HdrImage {
	int width, height;
	std::vector<Color> pixels;

	bool validCoordinates(int x, int y);
	int pixelOffset(int x, int y);
	Color getPixel(int x, int y);
	void setPixel(int x, int y, Color c);
	void savePfm(std::ofstream &stream, float value);
};
