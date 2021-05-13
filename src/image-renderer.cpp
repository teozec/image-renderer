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

#define USAGE \
	"Usage: " << endl << \
	programName << " -h|--help" << endl << \
	programName << " demo [options]" << endl << \
	programName << " pfm2ldr [options] <format>" \
	" <input-pfm-file> <output-file> " << endl

#define RUN_HELP \
	"Run '" << programName << " " << actionName << " -h' for all supported options." << endl

#define HELP_PFM2LDR \
	"Available actions:" << endl << \
	"-h, --help: print this message." << endl << endl << \
	"pfm2ldr: convert a pfm image to a ldr image in the desired format." << endl << \
	"	Common options:" << endl << \
	"		-a <value>, --afactor=<value>			Normalization factor." << endl << \
	"		-g <value>, --gamma=<value>			Gamma factor." << endl << endl << \
	"	Supported formats and related options:" << endl << \
	"		bmp" << endl << \
	"		gif" << endl << \
	"		jpeg 	-q <value>, --quality=<value>		Compression quality (0-95, 0 means default)." << endl << \
	"		png	-p, --palette				Whether to use palette or not."<< endl << \
	"			-c <value>, --compression=<value>	Compression level (0-9)." << endl << \
	"		tiff" << endl << \
	"		webp	-q <value>, --quality=<value>		Compression quality (0-100)." << endl

#define HELP_DEMO \
	"Available actions:" << endl << \
	"-h, --help: print this message." << endl << endl << \
	"demo: render a demo pfm image and write it to a PFM file (default: 'demo.pfm')." << endl << endl << \
	"	Common options:" << endl << \
	"		-w <value>, --width=<value>			Width of the final image." << endl << \
	"		-h <value>, --height=<value>			Height of the final image." << endl << \
	"		-p <string>, --projection=<string>		Projection used (default 'perspective')." <<endl << \
	"		--angleDeg=<value>				Angle of rotation (on z axis) of the camera." <<endl << \
	"		-o <string>, --outfile=<string>			Filename of the output image." << endl

using namespace std;

enum class ImageFormat { png, webp, jpeg , tiff, bmp, gif };

int demo(argh::parser cmdl);
int pfm2ldr(argh::parser cmdl);
void animation(int width, int height); //additional argument CameraProjection camProj

int main(int argc, char *argv[])
{
	argh::parser cmdl;

	cmdl.add_params({"-a", "--afactor",
			 "-g", "--gamma",
			 "-c", "--compression",
			 "-q", "--quality",
			 "-w", "--width",
			 "-h", "--height",
			 "-p", "--projection",
			 "--angleDeg",
			 "-o", "--outfile"});
	cmdl.parse(argc, argv);

	const string programName = cmdl[0];
	const string actionName = cmdl[1];
	
	// Parse action
	if (actionName == "demo") { 
		return demo(cmdl);
	} else if (actionName == "pfm2ldr") {
		return pfm2ldr(cmdl);
	} else if (cmdl[{"-h", "--help"}]) {
		cout << USAGE;
		return 0;
	} else {
		cout << USAGE << endl << RUN_HELP;
	}
}

int pfm2ldr(argh::parser cmdl)
{
	const string programName = cmdl[0];
	const string actionName = cmdl[1];

	if (cmdl[{"-h", "--help"}]) {
		cerr << HELP_PFM2LDR <<endl;
		return 0;
	}

	if (cmdl.size() != 5) {
		cerr << USAGE << endl << RUN_HELP;
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

int demo(argh::parser cmdl) {

	if (cmdl[{"-h", "--help"}]) {
		cerr << HELP_DEMO <<endl;
		return 0;
	}
	int width, height;
	cmdl({"-w", "--width"}, 300) >> width;
	cmdl({"-h", "--height"}, 200) >> height;
	float aspectRatio = width/height;

	string projString;
	int angle;
	cmdl({"-p", "--projection"}, "perspective") >> projString;
	cmdl({"--angleDeg"}, 0) >> angle;
	Transformation camTransformation{rotationZ(angle*M_PI/180)*translation(Vec{-1.f, 0.f, 0.f})};
	shared_ptr<Camera> cam;
	if (projString == "orthogonal")
		cam = make_shared<OrthogonalCamera>(OrthogonalCamera{aspectRatio, camTransformation});
	else if (projString == "perspective")
		cam = make_shared<PerspectiveCamera>(PerspectiveCamera{aspectRatio, camTransformation});
	else {
		cout << "Projection unrecognized. Try \"orthogonal\" or \"perspective\"." <<endl;
		return 1;
	}
	string ofilename;
	cmdl({"-o", "--output"}, "demo.pfm") >> ofilename;

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

	ImageTracer tracer{image, *cam};
	Color col[3] = {Color{1.f, 0.f, 0.f}, Color{0.f, 1.f, 0.f}, Color{0.f, 0.f, 1.f}};
	tracer.fireAllRays([&world, &col](Ray ray) {
		HitRecord record = world.rayIntersection(ray);
		int counter = 0;
		if (record.hit){
			return Color{5.f, 2.f, 0.f};
		} else
			return Color{0.01, 0.03, 0.1};
	});

	ofstream outPfm;
	outPfm.open(ofilename);
	image.writePfm(outPfm);
	outPfm.close();

	return 0;
}
