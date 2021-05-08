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
#include "shape.h"
#include "argh.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <unistd.h>

#define USAGE \
	"Usage: " << programName << " [options] <format>" \
	" <input-pfm-file> <output-file> " << endl << \
	"\nRun '" << programName << " -h' for all supported options." <<endl

#define HELP \
	"Common options:" << endl << \
	"	-h, --help					Print this message." <<endl << \
	"	-a <value>, --afactor=<value>			Normalization factor." << endl << \
	"	-g <value>, --gamma=<value>			Gamma factor." << endl << endl << \
	"Supported formats and related options:" << endl << \
	"	bmp" << endl << \
	"	gif" << endl << \
	"	jpeg 	-q <value>, --quality=<value>		Compression quality (0-95, 0 means default)." << endl << \
	"	png	-p, --palette				Whether to use palette or not."<< endl << \
	"		-c <value>, --compression=<value>	Compression level (0-9)." << endl << \
	"	tiff" << endl << \
	"	webp	-q <value>, --quality=<value>		Compression quality (0-100)." << endl

using namespace std;

enum class ImageFormat { png, webp, jpeg , tiff, bmp, gif };

enum class CameraProjection { orthogonal, perspective };

void makeCam (shared_ptr<Camera> cam, CameraProjection proj, float a, Transformation camT);
void demo(int width, int height); //additional argument CameraProjection camProj

int main(int argc, char *argv[])
{
	argh::parser cmdl;

	cmdl.add_params({"-a", "--afactor",
			 "-g", "--gamma",
			 "-c", "--compression",
			 "-q", "--quality"});
	cmdl.parse(argc, argv);

	const string programName = cmdl[0];
	const string actionName = cmdl[1];

	
	// Parse action
	if (actionName == "demo") { 
		demo(200, 100);
		return 0;
		} else if (actionName != "pfm2ldr") {
			cout << USAGE <<endl << HELP;
			return 0;
		}

	if (cmdl[{"-h", "--help"}]) {
		cout << USAGE << endl << HELP;
		return 0;
	}
	
	if (cmdl.size() != 5) {
		cerr << USAGE;
		return 1;
	}

	// Parse format
	const string formatStr = cmdl[2]; 
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

	const char *infile = cmdl[3].c_str();
	const char *outfile = cmdl[4].c_str();

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
			img.writeBmp(outfile, gamma);
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

void demo(int width, int height) {
	HdrImage image{width, height};

	World world;
	for(int i{}; i<2; i++){
		for(int j{}; j<2; j++){
			for(int k{}; k<2; k++)
				world.add(Sphere{translation(Vec{(float)(0.5-i), (float)(0.5-j), (float)(0.5-k)})*scaling(0.1, 0.1, 0.1)});
		}
	}

	world.add(Sphere{translation(Vec{0.f, 0.f, -0.5})*scaling(0.1, 0.1, 0.1)});
	world.add(Sphere{translation(Vec{0.f, 0.5, 0.f})*scaling(0.1, 0.1, 0.1)});

	Transformation camTransformation{translation(Vec{-1.f, 0.f, 0.f})};//try rotations
	float aspectRatio = width/height;
	//shared_ptr<Camera> cam;
	//makeCam(cam, camProj, aspectRatio, camTransformation);
	PerspectiveCamera cam{aspectRatio, camTransformation};
	ImageTracer tracer{image, cam};
	tracer.fireAllRays([&world](Ray ray) {
		if (world.rayIntersection(ray).hit)
			return Color{1.f, 1.f, 1.f};
		else
			return Color{0.f, 0.f, 0.f};
	});

	ofstream outPfm;
	outPfm.open("demo.pfm");
	image.writePfm(outPfm);
	outPfm.close();
}

void makeCam (shared_ptr<Camera> cam, CameraProjection proj, float a, Transformation camT){
	if(proj == CameraProjection::orthogonal)
		OrthogonalCamera cam{a, camT};
	else if (proj == CameraProjection::perspective)
		PerspectiveCamera cam{a, camT};
}