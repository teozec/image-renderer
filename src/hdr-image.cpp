#include <sstream>
#include <cstdint>
#include <iomanip>
#include "hdr-image.h"

using namespace std;

enum class Endianness { littleEndian, bigEndian };

static void writeFloat(stringstream &stream, const float value, const Endianness endianness) {

	// Convert "value" in a sequence of 32 bit
	uint32_t doubleWord{*((uint32_t *)&value)};

	// Extract the four bytes in "doubleWord" using bit-level operators
	uint8_t bytes[] = {
			static_cast<uint8_t>(doubleWord & 0xFF),
			static_cast<uint8_t>((doubleWord >> 8) & 0xFF),
			static_cast<uint8_t>((doubleWord >> 16) & 0xFF),
			static_cast<uint8_t>((doubleWord >> 24) & 0xFF),
	};

	switch (endianness) {
	case Endianness::littleEndian:
		for (int i{}; i < 4; ++i)
			stream << bytes[i];
		break;

	case Endianness::bigEndian:
		for (int i{3}; i >= 0; --i)
			stream << bytes[i];
		break;
	}
}

bool isLittleEndian() {
	uint16_t word{0x1234};
	uint8_t *ptr{(uint8_t *)&word};
	return ptr[0] == 0x34;
}

void HdrImage::savePfm(stringstream &stream) {

	//Define the endianness to use
	Endianness endianness;
	float endiannessFloat; // oppure const e richiamare funzione?
	if(isLittleEndian()){
		endianness = Endianness::littleEndian;
		endiannessFloat = -1.f;
	}
	else {
		endianness = Endianness::bigEndian;
		endiannessFloat = 1.f;
	}

	//Write PFM file
	stream	<< "PF\n" << width << " " << height << "\n"
			<< fixed << setprecision(1) << endiannessFloat << endl;

	for (int y = height-1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			Color c = getPixel(x, y);
			writeFloat(stream, c.r, endianness);
			writeFloat(stream, c.g, endianness);
			writeFloat(stream, c.b, endianness);
		}
	}
}
