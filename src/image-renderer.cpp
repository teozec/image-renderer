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

#define USAGE \
	"Usage: " << programName << " format" \
	" [-a a_factor] [-g gamma] [-p] [-c compression] [-q quality]" << \
	" input_file output_file " << endl << \
	"       " << programName << " -h" << endl

#define HELP \
	"Common options:" << endl << \
	"	-a	normalization factor" << endl << \
	"	-g	gamma factor" << endl << endl << \
	"Supported formats:" << endl << \
	"	bmp 	-c	compression level (0-1)" << endl << \
	"	gif" << endl << \
	"	jpeg 	-q	quality (0-95)" << endl << \
	"	png	-p	(enable palette)" << endl << \
	"		-c	compression level (0-9)" << endl << \
	"	tiff" << endl << \
	"	webp	-q	quality (0-100)" << endl

using namespace std;

enum class ImageFormat { png, webp, jpeg , tiff, bmp, gif };

int main(int argc, char *argv[])
{
	char *programName = argv[0];

	if (argc < 2) {
		cerr << USAGE;
		return 1;
	}

	// Parse format
	ImageFormat format;
	if (!strcmp(argv[1], "-h")) {
		cout << USAGE << endl << HELP;
		return 0;
	} else if (!strcmp(argv[1], "png")) {
		format = ImageFormat::png;
	} else if (!strcmp(argv[1], "webp")) {
		format = ImageFormat::webp;
	} else if (!strcmp(argv[1], "jpeg")) {
		format = ImageFormat::jpeg;
	} else if (!strcmp(argv[1], "tiff")) {
		format = ImageFormat::tiff;
	} else if (!strcmp(argv[1], "bmp")) {
		format = ImageFormat::bmp;
	} else if (!strcmp(argv[1], "gif")) {
		format = ImageFormat::gif;
	} else {
		cerr << "Format " << argv[1] <<
			" not supported" << endl;
		return 1;
	}
	argv++;
	argc--;

	// Define variables with their default values.
	bool palette = false;
	int compression = -1;
	int quality = -1;
	float aFactor = 0.18f;
	float gamma = 1.f;

	// Parse command line options (see USAGE)
	int c;
	while ((c = getopt(argc, argv, "pc:q:a:g:")) != -1) {
		switch (c) {
		case 'p':
			if (format != ImageFormat::png) {
				cerr << USAGE;
				return 1;
			}
			palette = true;
			break;
		case 'c':
			if (format != ImageFormat::png and format != ImageFormat::bmp) {
				cerr << USAGE;
				return 1;
			}
			try {
				compression = stoi(optarg);
			} catch (exception e) {
				cerr << USAGE;
				return 1;
			}
			break;
		case 'q':
			if (format != ImageFormat::jpeg and format != ImageFormat::webp) {
				cerr << USAGE;
				return 1;
			}
			try {
				quality = stoi(optarg);
			} catch (exception e) {
				cerr << USAGE;
				return 1;
			}
			break;
		case 'a':
			try {
				aFactor = stof(optarg);
			} catch (exception e) {
				cerr << USAGE;
				return 1;
			}
			break;
		case 'g':
			try {
				gamma = stof(optarg);
			} catch (exception e) {
				cerr << USAGE;
				return 1;
			}
			break;
		case '?':
			cerr << USAGE;
			return 1;
		}
	}

	// After the options, exactly two arguments must remain:
	// input_file and output_file
	if (argc - optind != 2) {
		cerr << USAGE;
		return 1;
	}

	const char *infile = argv[optind];
	const char *outfile = argv[optind+1];

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
			img.writeWebp(outfile, quality, gamma);
			break;
		case ImageFormat::jpeg:
			img.writeJpeg(outfile, quality, gamma);
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
