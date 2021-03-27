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

#include <cstdio>
#include <fstream>

using namespace std;

int main() {
	HdrImage img{7, 4};

	// Test constructor
	assert(img.width == 7);
	assert(img.height == 4);
	// Further tests available on readPfmFile section

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
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception e) {
		assert(false);
	}

	try {
		parseImageSize("ciao", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception e) {
		assert(false);
	}

	try {
		parseImageSize("1 3 4", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception e) {
		assert(false);
	}

	try {
		parseImageSize(" 3 4", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception e) {
		assert(false);
	}

	try {
		parseImageSize("3 4 ", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception e) {
		assert(false);
	}


	const char leRef[] = {
		'\x50', '\x46', '\x0a', '\x33', '\x20', '\x32', '\x0a', '\x2d', '\x31', '\x2e', '\x30', '\x0a',
		'\x00', '\x00', '\xc8', '\x42', '\x00', '\x00', '\x48', '\x43', '\x00', '\x00', '\x96', '\x43',
		'\x00', '\x00', '\xc8', '\x43', '\x00', '\x00', '\xfa', '\x43', '\x00', '\x00', '\x16', '\x44',
		'\x00', '\x00', '\x2f', '\x44', '\x00', '\x00', '\x48', '\x44', '\x00', '\x00', '\x61', '\x44',
		'\x00', '\x00', '\x20', '\x41', '\x00', '\x00', '\xa0', '\x41', '\x00', '\x00', '\xf0', '\x41',
		'\x00', '\x00', '\x20', '\x42', '\x00', '\x00', '\x48', '\x42', '\x00', '\x00', '\x70', '\x42',
		'\x00', '\x00', '\x8c', '\x42', '\x00', '\x00', '\xa0', '\x42', '\x00', '\x00', '\xb4', '\x42',
	};
	const int leLen = 84;

	const char beRef[] = {
		'\x50', '\x46', '\x0a', '\x33', '\x20', '\x32', '\x0a', '\x31', '\x2e', '\x30', '\x0a', '\x42',
		'\xc8', '\x00', '\x00', '\x43', '\x48', '\x00', '\x00', '\x43', '\x96', '\x00', '\x00', '\x43',
		'\xc8', '\x00', '\x00', '\x43', '\xfa', '\x00', '\x00', '\x44', '\x16', '\x00', '\x00', '\x44',
		'\x2f', '\x00', '\x00', '\x44', '\x48', '\x00', '\x00', '\x44', '\x61', '\x00', '\x00', '\x41',
		'\x20', '\x00', '\x00', '\x41', '\xa0', '\x00', '\x00', '\x41', '\xf0', '\x00', '\x00', '\x42',
		'\x20', '\x00', '\x00', '\x42', '\x48', '\x00', '\x00', '\x42', '\x70', '\x00', '\x00', '\x42',
		'\x8c', '\x00', '\x00', '\x42', '\xa0', '\x00', '\x00', '\x42', '\xb4', '\x00', '\x00'
	};
	const int beLen = 83;

	stringstream leInStream;
	leInStream.write(leRef, leLen);
	HdrImage leImg{leInStream};
	stringstream leOutStream;
	leImg.savePfm(leOutStream);
	char leBuf[leLen];
	leOutStream.read(leBuf, leLen);
	assert(!memcmp(leBuf, leRef, leLen));
	assert(leImg.width == 3);
	assert(leImg.height == 2);
	assert(leImg.getPixel(0,0) == (Color{1.0e1, 2.0e1, 3.0e1}));

	stringstream beInStream;
	beInStream.write(beRef, beLen);
	HdrImage beImg{beInStream};
	stringstream beOutStream;
	beImg.savePfm(beOutStream, Endianness::bigEndian);
	char beBuf[beLen];
	beOutStream.read(beBuf, beLen);
	assert(!memcmp(beBuf, beRef, beLen));
	assert(beImg.width == 3);
	assert(beImg.height == 2);
	assert(beImg.getPixel(0,0) == (Color{1.0e1, 2.0e1, 3.0e1}));

	return 0;
}
