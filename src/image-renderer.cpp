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
#include "argh.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <unistd.h>

#define USAGE \
	"Usage: " << programName << " [options] format" \
	" input_file output_file " << endl << \
	"       " << programName << " -h|--help" << endl

#define HELP \
	"Common options:" << endl << \
	"	-a, --afactor	<normalization factor>" << endl << \
	"	-g, --gamma	<gamma factor>" << endl << endl << \
	"Supported formats and per-format options:" << endl << \
	"	bmp 	-c, --compression	<compression level (0-1)>" << endl << \
	"	gif" << endl << \
	"	jpeg 	-q, --quality		<quality (0-95)>" << endl << \
	"	png	-p, --palette" << endl << \
	"		-c, --compression	<compression level (0-9)>" << endl << \
	"	tiff" << endl << \
	"	webp	-q, --quality		<quality (0-100)>" << endl

using namespace std;

enum class ImageFormat { png, webp, jpeg , tiff, bmp, gif };

int main(int argc, char *argv[])
{
	argh::parser cmdl;

	cmdl.add_params({"-a", "--afactor",
			 "-g", "--gamma",
			 "-c", "--compression",
			 "-q", "--quality"});
	cmdl.parse(argc, argv);

	const string programName = cmdl[0];

	if (cmdl[{"-h", "--help"}]) {
		cout << USAGE << endl << HELP;
		return 0;
	}

	if (cmdl.size() != 4) {
		cerr << USAGE;
		return 1;
	}

	// Parse format
	const string formatStr = cmdl[1]; 
	ImageFormat format;
	if (formatStr == "png") {
		format = ImageFormat::png;
	} else if (formatStr == "webp") {
		format = ImageFormat::webp;
	} else if (formatStr == "jpeg") {
		format = ImageFormat::jpeg;
	} else if (formatStr == "tiff") {
		format = ImageFormat::tiff;
	} else if (formatStr == "bmp") {
		format = ImageFormat::bmp;
	} else if (formatStr == "gif") {
		format = ImageFormat::gif;
	} else {
		cerr << "Format " << formatStr <<
			" not supported" << endl;
		return 1;
	}

	const char *infile = cmdl[2].c_str();
	const char *outfile = cmdl[3].c_str();

	// Parse parameters (also specifying default values).
	float aFactor;
	cmdl({"-a", "--afactor"}, 0.18f) >> aFactor;

	float gamma = 1.f;
	cmdl({"-g", "--gamma"}, 1.f) >> gamma;

	bool palette = cmdl[{"-p", "--palette"}];

	int compression;
	cmdl({"-c", "--compression"}, -1) >> compression;

	int quality;
	cmdl({"-q", "--quality"}, -1) >> quality;

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
