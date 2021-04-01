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
		" [-p] [-a a_factor] [-g gamma] [-c compression/quality]" << \
		" format input_file output_file " << \
		endl

using namespace std;

enum class ImageFormat { png, webp, jpeg , tiff, bmp, gif };

int main(int argc, char *argv[])
{
	// Define variables with their default values.
	bool palette = false;
	int compression = -1;
	float aFactor = 0.18f;
	float gamma = 1.f;

	// Parse command line options (see USAGE)
	int c;
	while ((c = getopt(argc, argv, "pc:a:g:")) != -1) {
		switch (c) {
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

	// After the options, exactly three arguments must remain:
	// format, input_file and output_file
	if (argc - optind != 3) {
		USAGE();
		return 1;
	}
	
	// Parse format
	ImageFormat format;
	if (!strcmp(argv[optind], "png")) {
		format = ImageFormat::png;
	} else if (!strcmp(argv[optind], "webp")) {
		format = ImageFormat::webp;
	} else if (!strcmp(argv[optind], "jpeg")) {
		format = ImageFormat::jpeg;
	} else if (!strcmp(argv[optind], "tiff")) {
		format = ImageFormat::tiff;
	} else if (!strcmp(argv[optind], "bmp")) {
		format = ImageFormat::bmp;
	} else if (!strcmp(argv[optind], "gif")) {
		format = ImageFormat::gif;
	} else {
		cerr << "Format " << optarg <<
			" not supported" << endl;
		return 1;
	}

	const char *infile = argv[optind+1];
	const char *outfile = argv[optind+2];

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
		case ImageFormat::webp:
			img.writeWebp(outfile, compression, gamma);
			break;
		case ImageFormat::jpeg:
			img.writeJpeg(outfile, compression, gamma);
			break;
		case ImageFormat::tiff:
			img.writeTiff(outfile, gamma);
			break;
		case ImageFormat::bmp:
			img.writeBmp(outfile, compression, gamma);
			break;
		case ImageFormat::gif:
			img.writeGif(outfile, gamma);
			break;
		}
	} catch (runtime_error e) {
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}
