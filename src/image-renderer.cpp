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
#include <iostream>
#include <iomanip>
#include <unistd.h>

#define USAGE() \
	cerr << "Usage: " << argv[0] << \
		" -f format -i input_file -o output_file " \
		"[-p] [-a aFactor] [-g gamma]" << \
		endl;

using namespace std;

enum class ImageFormat { png };

int main(int argc, char *argv[])
{
	// Define variables with their default values.
	ImageFormat format;
	bool formatDefined = false;
	char *infile = NULL;
	char *outfile = NULL;
	bool palette = false;
	int compression = -1;
	float aFactor = 0.18f;
	float gamma = 1.f;

	// Parse command line arguments (see USAGE)
	int c;
	while ((c = getopt(argc, argv, "f:i:o:pc:a:g:")) != -1) {
		switch (c) {
		case 'f':
			if (!strcmp(optarg, "png")) {
				format = ImageFormat::png;
			} else {
				cerr << "Format " << optarg <<
					" not supported" << endl;
				return 1;
			}
			formatDefined = true;
			break;
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'p':
			palette = true;
			break;
		case 'c':
			try {
				compression = stoi(optarg);
			} catch (exception e) {
				USAGE();
				return 1;
			}
			break;
		case 'a':
			try {
				aFactor = stof(optarg);
			} catch (exception e) {
				USAGE();
				return 1;
			}
			break;
		case 'g':
			try {
				gamma = stof(optarg);
			} catch (exception e) {
				USAGE();
				return 1;
			}
			break;
		case '?':
			USAGE();
			return 1;
		}
	}

	// -i, -o and -f are mandatory options
	if (!infile or !outfile or !formatDefined) {
		USAGE();
		return 1;
	}

	// Read the input file
	HdrImage img;
	try {
		img.readPfm(infile);
	} catch (exception e) {
		cerr << "Error: " <<  e.what() << endl;
		return 1;
	}

	// Convert HDR to LDR
	img.normalizeImage(aFactor);
	img.clampImage();

	// Write to output file
	try {
		switch (format) {
		case ImageFormat::png:
			img.writePng(outfile, compression, palette, gamma);
			break;
		}
	} catch (runtime_error e) {
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}
