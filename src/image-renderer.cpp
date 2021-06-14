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

#include "renderer.h"
#include "argh.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>

#define USAGE \
	"Usage: " << endl << \
	programName << " -h|--help" << endl << \
	programName << " demo [options]" << endl << \
	programName << " pfm2ldr [options] <format> <inputfile> <outputfile>" << \
	programName << " stack [options] <inputfiles>" \
	" <input-pfm-file> <output-file> " << endl

#define RUN_HELP \
	"Run '" << programName << " <action-name> -h' for all supported options." << endl

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
	"		--antialiasing=<value>				Number of samples per single pixel (must be a perfect square, e.g. 4)." <<endl << \
	"		-s <value>, --seed=<value>			Random number generator seed." << endl << \
	"		-o <string>, --outfile=<string>			Filename of the output image." << endl

#define HELP_STACK \
	"stack: stack more pfm images representing the same scene." << endl << endl << \
	"Available options:" << endl << \
	"	-h, --help: print this message." << endl << \
	"	-m <string>, --method=<string>			The stacking method (can be 'mean' or 'median', default 'mean')." << endl << \
	"	-S <value>, --nSigma=<value>			Number of sigma clipping iterations (default 0)." << endl << \
	"	-a <value>, --alpha=<value>			Sigma clipping alpha factor (consider outliers values farther than alpha*sigma from the median, default 2)." << \
	"	-o <string>, --outfile=<string>			Filename of the output image." << endl

using namespace std;

enum class ImageFormat { png, webp, jpeg, tiff, bmp, gif };

int demo(argh::parser cmdl);
int pfm2ldr(argh::parser cmdl);
int stackPfm(argh::parser cmdl);

int main(int argc, char *argv[])
{
	argh::parser cmdl;

	cmdl.add_params({"-a", "--afactor", "--alpha",
			 "-g", "--gamma",
			 "-c", "--compression",
			 "-q", "--quality",
			 "-w", "--width",
			 "-h", "--height",
			 "-p", "--projection",
			 "--angleDeg",
			 "--antialiasing",
			 "-o", "--outfile",
			 "-s", "--seed",
			 "-S", "--nSigma",
			 "-m", "--method"});
	cmdl.parse(argc, argv);

	const string programName = cmdl[0];
	const string actionName = cmdl[1];
	
	// Parse action
	if (actionName == "demo") { 
		return demo(cmdl);
	} else if (actionName == "pfm2ldr") {
		return pfm2ldr(cmdl);
	} else if (actionName == "stack") {
		return stackPfm(cmdl);
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
	cmdl({"-a", "--afactor"}, 0.3f) >> aFactor;

	float gamma;
	cmdl({"-g", "--gamma"}, .7f) >> gamma;

	bool palette = cmdl[{"-p", "--palette"}];

	int compression;
	cmdl({"-c", "--compression"}, -1) >> compression;

	int quality;
	cmdl({"-q", "--quality"}, -1) >> quality;

	// Read the input file
	HdrImage img;
	try {
		img.readPfm(infile);
	} catch (exception &e) {
		cerr << "Error: " <<  e.what() << endl;
		return 1;
	}

	// Convert HDR to LDR
	img.normalizeImage(aFactor, 0.25f);
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
	cmdl({"-w", "--width"}, 1000) >> width;
	cmdl({"-h", "--height"}, 1000) >> height;
	float aspectRatio = (float) width / height;

	Material material1{SpecularBRDF(UniformPigment(Color{.7f, .3f, .2f}))};

	Material material2{DiffusiveBRDF(CheckeredPigment(Color{.7f, .8f, .5f}, Color{.7f, .2f, .3f}, 4))};

	Material materialSky{DiffusiveBRDF{UniformPigment(Color{.5f, .8f, 1.f})}, UniformPigment{Color{.5f, .9f, 1.f}}};
	Material materialGround{DiffusiveBRDF(CheckeredPigment(Color{.3f, .5f, .1f}, Color{.8f, .8f, .8f}, 2))};

	string projString;
	int angle;
	cmdl({"-p", "--projection"}, "perspective") >> projString;
	int seed;
	cmdl({"-s", "--seed"}, 42) >> seed;
	cmdl({"--angleDeg"}, 0) >> angle;
	Transformation camTransformation{rotationZ((angle + .25f)*M_PI/180)*translation(Vec{-1.f, 0.f, 0.f})};
	shared_ptr<Camera> cam;
	if (projString == "orthogonal")
		cam = make_shared<OrthogonalCamera>(OrthogonalCamera{aspectRatio, camTransformation});
	else if (projString == "perspective")
		cam = make_shared<PerspectiveCamera>(PerspectiveCamera{aspectRatio, camTransformation});
	else {
		cerr << "Projection unrecognized. Try 'orthogonal' or 'perspective'." <<endl;
		return 1;
	}

	HdrImage image{width, height};
	World world;

	world.add(Sphere{translation(Vec{1.2f, -1.1f, 0.f}), material1});
	world.add(Sphere{translation(Vec{0.f, .6f, 0.f}), material2});

	world.add(Sphere{scaling(5.f), materialSky});
	world.add(Plane{translation(Vec{0.f, 0.f, -1.f}), materialGround});

	int samplesPerPixel;
	cmdl({"--antialiasing"}, 0) >> samplesPerPixel;
	int samplesPerSide = sqrt(samplesPerPixel);
	if (samplesPerPixel != samplesPerSide*samplesPerSide){
		cerr << "Not a perfect square given as --antialiasing parameter."  <<endl;
		return 1;
	} 
	ImageTracer tracer{image, *cam, samplesPerSide};
	PCG pcg{(uint64_t) seed};

	tracer.fireAllRays(PathTracer{world, pcg, 5, 4, 4});

	string ofilename;
	cmdl({"-o", "--outfile"}, "demo.pfm") >> ofilename;
	ofstream outPfm;
	outPfm.open(ofilename);
	image.writePfm(outPfm);
	outPfm.close();

	return 0;

}

int stackPfm(argh::parser cmdl)
{
	const string programName = cmdl[0];
	const string actionName = cmdl[1];
	if (cmdl[{"-h", "--help"}]) {
		cerr << HELP_STACK << endl;
		return 0;
	}

	if (cmdl.size() < 3) {
		cerr << USAGE << endl << HELP_STACK;
		return 1;
	}

	string method;
	cmdl({"-m", "--method"}, "mean") >> method;
	if (method != "mean" and method != "median") {
		cerr << "Error: method " << method << " not supported." << endl;
		return 1;
	}
	string ofilename;
	cmdl({"-o", "--outfile"}, "stack.pfm") >> ofilename;
	int nSigmaIterations;
	cmdl({"-S", "--nSigma"}, 0) >> nSigmaIterations;
	float alpha;
	cmdl({"-a", "--alpha"}, 2.) >> alpha;

	HdrImage firstImg;
	try {
		firstImg.readPfm(cmdl[2]);
	} catch (exception &e) {
		cerr << "Error: " <<  e.what() << endl;
		return 1;
	}

	const int width = firstImg.width, height = firstImg.height;
	HdrImage stackedImage{width, height};

	// Make a vector for the pixels, each element of which contains a vector for the 3 colors, each element of which contains a vector for the images.
	// i.e.: imgVector[pixel][color][image] is the value of the color "color" at pixel "pixel" for the image "image".
	// We don't use the Color struct because we need to treat each color for each pixel for each image independently, to sort and remove them.
	vector<vector<vector<float>>> imgVector{static_cast<size_t>(height * width)};

	// Each pixel is a vector of three colors.
	for (int pixel{}; pixel < height * width; pixel++)
		imgVector[pixel].resize(3);

	// Process each image in sequence.
	for (int i{2}; i < cmdl.size(); i++) {
		HdrImage img;
		string imageName = cmdl[i];

		try {
			img.readPfm(imageName);
		} catch (exception &e) {
			cerr << "Error: " <<  e.what() << endl;
			return 1;
		}

		// All the images to stack must have the same height and width.
		if (img.width != width or img.height != height) {
			cerr << "Error: " << imageName << " has not the same size as " << cmdl[2] << endl;
			return 1;
		}

		// Add the 3 colors of all the pixels of the image to imgVector, keeping each [pixel][color] vector (containing all the images) sorted.
		for (int pixel{}; pixel < height * width; pixel++) {
			for (int color{}; color < 3; color++) {
				imgVector[pixel][color].push_back(img.pixels[pixel][color]);
				inplace_merge(imgVector[pixel][color].begin(), imgVector[pixel][color].end() - 1, imgVector[pixel][color].end());
			}
		}
	}

	for (int i{}; i < nSigmaIterations; i++) {
		for (int pixel{}; pixel < height * width; pixel++) {
			for (int color{}; color < 3; color++) {
				auto &images = imgVector[pixel][color];
				size_t size = images.size();

				// Compute sigma
				float mean{}, mean2{};
				for (int img{}; img < size; img++) {
					mean += images[img];
					mean2 += images[img] * images[img];
				}
				mean /= (float) size;
				mean2 /= (float) size;
				float sigma = sqrt(mean2 - mean * mean);

				// Compute median
				float median;
				if (size % 2 == 0)
					median = (images[size / 2 - 1] + images[size / 2]) / 2.f;
				else
					median = images[size / 2];

				// Remove all the outliers x for which |median - x| > alpha * sigma
				for (auto it{images.begin()}; it != images.end(); ) {
					if (abs(*it - median) > alpha * sigma)
						it = images.erase(it);
					else
						it++;
				}
			}
		}
	}

	if (method == "mean") {
		for (int pixel{}; pixel < height * width; pixel++) {
			for (int color{}; color < 3; color++) {
				size_t size = imgVector[pixel][color].size();
				for (int img{}; img < size; img++)
					stackedImage.pixels[pixel][color] += imgVector[pixel][color][img];
				stackedImage.pixels[pixel][color] /= (float) size;
			}
		}
	} else if (method == "median") {
		for (int pixel{}; pixel < height * width; pixel++) {
			for (int color{}; color < 3; color++) {
				size_t size = imgVector[pixel][color].size();
				if (size % 2 == 0)
					stackedImage.pixels[pixel][color] = (imgVector[pixel][color][size / 2 - 1] + imgVector[pixel][color][size / 2]) / 2.f;
				else
					stackedImage.pixels[pixel][color] = imgVector[pixel][color][size / 2];
			}
		}
	}

	ofstream outPfm;
	outPfm.open(ofilename);
	stackedImage.writePfm(outPfm);
	outPfm.close();

	return 0;
}
