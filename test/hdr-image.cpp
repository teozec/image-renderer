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
#include <iostream>
#include <fstream>

using namespace std;

void testAverageLuminosity();

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
	} catch (exception &e) {
		assert(false);
	}

	try {
		parseImageSize("ciao", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception &e) {
		assert(false);
	}

	try {
		parseImageSize("1 3 4", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception &e) {
		assert(false);
	}

	try {
		parseImageSize(" 3 4", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception &e) {
		assert(false);
	}

	try {
		parseImageSize("3 4 ", width, height);
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception &e) {
		assert(false);
	}


	// Test savePfm and readPfm (little endian)
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

	stringstream leInStream;
	leInStream.write(leRef, leLen);
	HdrImage leImg{leInStream}; //use of readPfmFile

	stringstream leOutStream;
	leImg.writePfm(leOutStream); //use of savePfm
	char leBuf[leLen];
	leOutStream.read(leBuf, leLen);

	//	testing the saved image
	assert(!memcmp(leBuf, leRef, leLen));
	//	testing the read image
	assert(leImg.width == 3);
	assert(leImg.height == 2);
	assert(leImg.getPixel(0,0) == (Color{1.0e1, 2.0e1, 3.0e1}));
	assert(leImg.getPixel(1,0) == (Color{4.0e1, 5.0e1, 6.0e1}));
	assert(leImg.getPixel(2,0) == (Color{7.0e1, 8.0e1, 9.0e1}));
	assert(leImg.getPixel(0,1) == (Color{1.0e2, 2.0e2, 3.0e2}));
	assert(leImg.getPixel(1,1) == (Color{4.0e2, 5.0e2, 6.0e2}));
	assert(leImg.getPixel(2,1) == (Color{7.0e2, 8.0e2, 9.0e2}));

	// Test savePfm and readPfm (big endian)
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

	stringstream beInStream;
	beInStream.write(beRef, beLen);
	HdrImage beImg{beInStream}; //use of readPfmFile

	stringstream beOutStream;
	beImg.writePfm(beOutStream, Endianness::bigEndian); //use of savePfm
	char beBuf[beLen];
	beOutStream.read(beBuf, beLen);

	//	testing the saved image
	assert(!memcmp(beBuf, beRef, beLen));
	//	testing the read image
	assert(beImg.width == 3);
	assert(beImg.height == 2);
	assert(beImg.getPixel(0,0) == (Color{1.0e1, 2.0e1, 3.0e1}));
	assert(beImg.getPixel(0,0) == (Color{1.0e1, 2.0e1, 3.0e1}));
	assert(beImg.getPixel(1,0) == (Color{4.0e1, 5.0e1, 6.0e1}));
	assert(beImg.getPixel(2,0) == (Color{7.0e1, 8.0e1, 9.0e1}));
	assert(beImg.getPixel(0,1) == (Color{1.0e2, 2.0e2, 3.0e2}));
	assert(beImg.getPixel(1,1) == (Color{4.0e2, 5.0e2, 6.0e2}));
	assert(beImg.getPixel(2,1) == (Color{7.0e2, 8.0e2, 9.0e2}));

	stringstream badPfm;
	const char fileContent[] = u8"PF\n3 2\n-1.0\nstop";
	badPfm.write(fileContent, sizeof(fileContent));
	try {
		HdrImage badImg{badPfm};
		assert(false);
	} catch (InvalidPfmFileFormat e) {
	} catch (exception &e) {
		assert(false);
	}

	// Test normalizeImage
	HdrImage imgN{2, 1};
	imgN.setPixel(0, 0, Color{0.5e1, 1.0e1, 1.5e1});
	imgN.setPixel(1, 0, Color{0.5e3, 1.0e3, 1.5e3});
	imgN.normalizeImage(1000.0, 100.0);//factor=1000. and luminosity=100.
	assert(imgN.getPixel(0, 0)==(Color{0.5e2, 1.0e2, 1.5e2}));
	assert(imgN.getPixel(1, 0)==(Color{0.5e4, 1.0e4, 1.5e4}));

	// Test clampImage
	imgN.clampImage();
	for (int i{}; i<imgN.pixels.size(); i++){
		assert((imgN.pixels[i].r >= 0) && (imgN.pixels[i].r <= 1));
		assert((imgN.pixels[i].g >= 0) && (imgN.pixels[i].g <= 1));
		assert((imgN.pixels[i].b >= 0) && (imgN.pixels[i].b <= 1));
	}

	// Test averageLuminosity
	testAverageLuminosity();

	return 0;
}

void testAverageLuminosity() {
	HdrImage img{2, 1};

	img.setPixel(0, 0, Color{5.f, 10.f, 15.f});
	img.setPixel(1, 0, Color{500.f, 1000.f, 1500.f});

	assert(img.averageLuminosity(0.f) == 100.f);
}
