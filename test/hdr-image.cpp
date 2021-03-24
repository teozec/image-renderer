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

#include "hdr-image.h"
#include "color.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <cstdint>

#include <fstream>

using namespace std;

int main() {
	HdrImage img{7, 4};

	// Test constructor
	assert(img.width == 7);
	assert(img.height == 4);

	// Test validCoordinates
	assert(img.validCoordinates(0, 0));
	assert(img.validCoordinates(6, 3));
	assert(!img.validCoordinates(7, 3));
	assert(!img.validCoordinates(6, 4));
	assert(!img.validCoordinates(-1, 3));
	assert(!img.validCoordinates(6, -1));

	// Test pixelOffset	
	assert(img.pixelOffset(0, 0) == 0);
	assert(img.pixelOffset(3, 2) == (3*4 + 2));

	// Test set&getPixel
	Color ref{1.0, 2.0, 3.0};
	img.setPixel(3, 2, ref);
	assert(img.getPixel(3, 2) == ref);

	// Test savePfm
	HdrImage img2{3, 2};
	img2.setPixel(0, 0, Color{1.0e1, 2.0e1, 3.0e1});
	img2.setPixel(1, 0, Color{4.0e1, 5.0e1, 6.0e1});
	img2.setPixel(2, 0, Color{7.0e1, 8.0e1, 9.0e1});
	img2.setPixel(0, 1, Color{1.0e2, 2.0e2, 3.0e2});
	img2.setPixel(1, 1, Color{4.0e2, 5.0e2, 6.0e2});
	img2.setPixel(2, 1, Color{7.0e2, 8.0e2, 9.0e2});

	const uint8_t pfmRef[] = {
		0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
		0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43,
		0x00, 0x00, 0xc8, 0x43, 0x00, 0x00, 0xfa, 0x43, 0x00, 0x00, 0x16, 0x44,
		0x00, 0x00, 0x2f, 0x44, 0x00, 0x00, 0x48, 0x44, 0x00, 0x00, 0x61, 0x44,
		0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xf0, 0x41,
		0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x70, 0x42,
		0x00, 0x00, 0x8c, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xb4, 0x42,
	};
	const int pfmLen = 84;
	stringstream buf;
	img2.savePfm(buf);

	char pfmBuf[pfmLen];
	buf.read(pfmBuf, pfmLen);
	assert(!memcmp(pfmBuf, pfmRef, pfmLen));

	// Test parseEndianness
	assert(parseEndianness("1.0") == Endianness::bigEndian);
	assert(parseEndianness("-1.0") == Endianness::littleEndian);
	try {
		parseEndianness("2.0");
	} catch (InvalidPfmFileFormat e) {
	}
	try {
		parseEndianness("ciao");
	} catch (InvalidPfmFileFormat e) {
	}
	
	// Test parseImageSize
	int width, height;
	parseImageSize("3 2", width, height);
	assert(width == 3 and height == 2);

	try {
		parseImageSize("-1 3", width, height);
	} catch (InvalidPfmFileFormat e) {
	}
	try {
		parseImageSize("ciao", width, height);
	} catch (InvalidPfmFileFormat e) {
	}

	return 0;
}
