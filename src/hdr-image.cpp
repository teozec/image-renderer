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

#include <ostream>
#include <istream>
#include <iomanip>
#include <cstdint>
#include <iomanip>
#include <string>
#include <cstdio>
#include <cstdarg>
#include <gd.h>
#include <gd_errors.h>
#include "hdr-image.h"

using namespace std;

static void writeFloat(ostream &stream, const float value, const Endianness endianness) {
	// Convert "value" to a sequence of 32 bit
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

static float readFloat(istream &stream, Endianness endianness) {
	uint8_t bytes[4];

	for (int i{}; i < 4; i++)
		stream >> noskipws >> bytes[i];

	float value = 0.f;
	switch (endianness) {
	case Endianness::littleEndian:
		for (int i{}; i < 4; ++i)
			*(uint32_t*)(&value) += ((uint32_t) bytes[i]) << (8*i);
		break;
	case Endianness::bigEndian:
		for (int i{}; i < 4; ++i)
			*(uint32_t*)(&value) += ((uint32_t) bytes[i]) << (8*(3-i));
		break;
	}
	return value;
}

static bool isLittleEndian() {
	uint16_t word{0x1234};
	uint8_t *ptr{(uint8_t *)&word};
	return ptr[0] == 0x34;
}

void parseImageSize(const string line, int &width, int &height) {
	char c;
	istringstream s(line);
	s >> noskipws >> width >> c >> height;
	if (s.fail() or !s.eof() or c != ' ' or width <= 0 or height <= 0)
		throw InvalidPfmFileFormat("Invalid size specification");
}

Endianness parseEndianness(const string line) {
	float endiannessFloat;
	try {
		endiannessFloat = stof(line);
	} catch (exception e) {
		throw InvalidPfmFileFormat("Invalid endianness specification");
	}
	if (endiannessFloat == 1.0f)
		return Endianness::bigEndian;
	else if (endiannessFloat == -1.0f)
		return Endianness::littleEndian;
	else
		throw InvalidPfmFileFormat("Invalid endianness specification");
}

void HdrImage::writePfm(ostream &stream, Endianness endianness) {
	//Define the endianness to use
	float endiannessFloat;
	switch (endianness) {
	case Endianness::littleEndian:
		endiannessFloat = -1.f;
		break;
	case Endianness::bigEndian:
		endiannessFloat = 1.f;
		break;
	}

	//Write PFM file
	stream	<< "PF\n" << width << ' ' << height << '\n'
			<< fixed << setprecision(1) << endiannessFloat << '\n';

	for (int y{height-1}; y >= 0; y--) {
		for (int x{}; x < width; x++) {
			Color c = getPixel(x, y);
			writeFloat(stream, c.r, endianness);
			writeFloat(stream, c.g, endianness);
			writeFloat(stream, c.b, endianness);
		}
	}
}

void HdrImage::readPfm(istream &stream) {
	// Measure length of file
	stream.seekg(0, stream.end);
	int fileLen = stream.tellg();
	stream.seekg(0, stream.beg);
	if (fileLen == -1)
		throw InvalidPfmFileFormat("The file is empty!");

	// Get the magic from PFM
	string magic;
	getline(stream, magic);
	if (magic != "PF")
		throw InvalidPfmFileFormat("Invalid magic in PFM file");
	
	// Get the width and height from PFM
	string imgSize;
	getline(stream, imgSize);
	parseImageSize(imgSize, width, height);

	// Get the endianness of PFM file
	string endStr;
	Endianness endianness;
	getline(stream, endStr);
	endianness = parseEndianness(endStr);

	// Checks file dimension
	int headerLen = stream.tellg();
	if ((fileLen-headerLen) != (width*height*3*4))
		throw InvalidPfmFileFormat("Invalid file dimension");

	// Get the actual image
	pixels.resize(width * height);
	for (int y{height-1}; y >= 0; y--) {
		for (int x{}; x < width; x++) {
			float r = readFloat(stream, endianness);
			float g = readFloat(stream, endianness);
			float b = readFloat(stream, endianness);
			setPixel(x, y, Color{r, g, b});
		}
	}
}

float clamp(const float x) {
	return x/(1+x);
};

// By default, gd writes error messages on stderr.
// We want to throw exceptions and let the caller handle them instead.
static void errorHandler(int priority, const char *format, va_list args) {
	const size_t size = 256;
	char msg[size] = {0};
	int n;

	// The first part of the messages is the error priority.
	switch (priority) {
	case GD_ERROR:
		n = snprintf(msg, size, "GD Error: ");
		break;
	case GD_WARNING:
		n = snprintf(msg, size, "GD Warning: ");
		break;
	case GD_NOTICE:
		n = snprintf(msg, size, "GD Notice: ");
		break;
	case GD_INFO:
		n = snprintf(msg, size, "GD Info: ");
		break;
	case GD_DEBUG:
		n = snprintf(msg, size, "GD Debug: ");
		break;
	}

	// The second part is the real error message content.
	if (n >= 0)
		n += vsnprintf(msg+n, size-n, format, args);
	if (msg[n-1] == '\n')
		msg[n-1] = 0;

	throw runtime_error{msg};
}

void HdrImage::writePng(const char filename[], int compression, bool palette, float gamma) {
	// Open output file, or throw exception on failure.
	FILE *f = fopen(filename, "wb");
	if (!f)
		throw runtime_error("Error: Could not open file");

	// Set errorHandler as the function to be called by gd if errors arise
	gdSetErrorMethod((gdErrorMethod) errorHandler);

	// Create a new true color image, or throw exception on failure
	// (On failure, the function returns NULL)
	gdImagePtr im = gdImageCreateTrueColor(width, height);
	if (!im)
		throw runtime_error{"Error: Failed to create gdImage"};

	// Set each pixel in the image, normalizing and applying the gamma factor
	for (int x{}; x < width; x++) {
		for (int y{}; y < height; y++) {
			Color p = getPixel(x, y);
			// Make a gd color index for the required color
			int index = gdImageColorExact(im,
				(int) 255 * pow(p.r, 1./gamma),
				(int) 255 * pow(p.g, 1./gamma),
				(int) 255 * pow(p.b, 1./gamma));

			gdImageSetPixel(im, x, y, index);
		}
	}

	// If the caller asked for a 8-bit palette PNG, try to convert the true color image.
	// On failure (the function returns 0), throw an exception.
	if (palette)
		if (!gdImageTrueColorToPalette(im, 0, 256))
			throw runtime_error{"Error: Failed to create palette"};

	// Write the image on the file, with the desired compression level.
	gdImagePngEx(im, f, compression);

	gdImageDestroy(im);
	fclose(f);
}

void HdrImage::writeWebp(const char filename[], int quality, float gamma) {
	// Open output file, or throw exception on failure.
	FILE *f = fopen(filename, "wb");
	if (!f)
		throw runtime_error("Error: Could not open file");

	// Set errorHandler as the function to be called by gd if errors arise
	gdSetErrorMethod((gdErrorMethod) errorHandler);

	// Create a new true color image, or throw exception on failure
	// (On failure, the function returns NULL)
	gdImagePtr im = gdImageCreateTrueColor(width, height);
	if (!im)
		throw runtime_error{"Error: Failed to create gdImage"};

	// Set each pixel in the image, normalizing and applying the gamma factor
	for (int x{}; x < width; x++) {
		for (int y{}; y < height; y++) {
			Color p = getPixel(x, y);
			// Make a gd color index for the required color
			int index = gdImageColorExact(im,
				(int) 255 * pow(p.r, 1./gamma),
				(int) 255 * pow(p.g, 1./gamma),
				(int) 255 * pow(p.b, 1./gamma));

			gdImageSetPixel(im, x, y, index);
		}
	}

	// Write the image on the file, with the desired compression level.
	gdImageWebpEx(im, f, quality);

	gdImageDestroy(im);
	fclose(f);
}
