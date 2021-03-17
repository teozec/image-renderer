#include <ostream>
#include <cstdint>
#include "hdr-image.h"

using namespace std;

static void writeFloat(ostream &stream, const float value) {
	const uint8_t *ptr{(const uint8_t *)&value};
	for (size_t i{}; i < sizeof value; i++)
		stream << ptr[i];
}

static bool isLittleEndian() {
	const uint16_t word{0x1234};
	const uint8_t *ptr{(const uint8_t *)&word};

	return ptr[0] == 0x34;
}

void HdrImage::savePfm(ostream &stream) {
	const float endianness = isLittleEndian() ? -1.0f : 1.0f;

	stream << "PF\n" << width << " " << height << "\n" << endianness;
	for (int y = height-1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			Color c = getPixel(x, y);
			writeFloat(stream, c.r);
			writeFloat(stream, c.g);
			writeFloat(stream, c.b);
		}
	}
}
