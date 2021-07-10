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
#include "parser.h"
#include "texture.h"
#include "argh.h"

#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>

#define USAGE \
	programName << ": a C++ tool to generate photo-realistic images." << endl << endl << \
	"Usage: " << endl << \
	programName << " -h|--help" << endl << \
	programName << " render [options] <inputfile>" << endl << \
	programName << " demo [options]" << endl << \
	programName << " pfm2ldr [options] <inputfile>" << endl << \
	programName << " stack [options] <inputfiles>" << endl

#define RUN_HELP \
	"Run '" << programName << " <action-name> -h|--help' for all supported options." << endl

#define HELP_PFM2LDR \
	"pfm2ldr: convert a pfm image to a ldr image in the desired format." << endl << endl << \
	"Usage:	" << programName << " pfm2ldr [options] <inputfile>" << endl << endl << \
	"General options:" << endl << \
	"	-h, --help					Print this message." << endl << \
	"	-f <format>, --format=<format>			Format of the output ldr image (default 'png'). See below for a list of supported formats." << endl << \
	"	-o <string>, --outfile=<string>			Filename of output image (default input filename with format-dependent extension)." << endl << \
	"	-l <value>, --luminosity=<value>		Total luminosity normalization factor (by default, it is calculated from the input pfm)." << endl << \
	"	-a <value>, --afactor=<value>			Normalization coefficient (default 0.3)." << endl << \
	"	-g <value>, --gamma=<value>			Gamma factor (default 1)." << endl << endl << \
	"Supported formats and related options:" << endl << \
	"	bmp" << endl << \
	"	gif" << endl << \
	"	jpeg 	-q <value>, --quality=<value>		Compression quality (0-95, 0 means default)." << endl << \
	"	png	-p, --palette				Whether to use palette or not."<< endl << \
	"		-c <value>, --compression=<value>	Compression level (0-9)." << endl << \
	"	tiff" << endl << \
	"	webp	-q <value>, --quality=<value>		Compression quality (0-100)." << endl

#define HELP_DEMO \
	"demo: render a demo pfm image." << endl << endl << \
	"Usage: " << programName << " demo [options] <inputfile>" << endl << endl << \
	"General options:" << endl << \
	"	-h, --help					Print this message." << endl << \
	"	-w <value>, --width=<value>			Width of the final image (default 600)." << endl << \
	"	-h <value>, --height=<value>			Height of the final image (default 400)." << endl << \
	"	-a <value>, --aspectRatio=<value>		Aspect ratio of the final image (default width/height)." << endl << \
	"	-p <string>, --projection=<string>		Projection used (default 'perspective'). Can be 'perspective' or 'orthogonal'" << endl << \
	"	-D <value>, --angleDeg=<value>			Angle of rotation (on z axis) of the camera (default 0)." << endl << \
	"	-A <value>, --antialiasing=<value>		Number of samples per single pixel (default 0). Must be a perfect square, e.g. 4." << endl << \
	"	-R <renderer>, --renderer=<renderer>		Rendering algorithm (default 'path'). Can be 'path', 'debug', 'onoff', 'flat'." << endl << \
	"	-o <string>, --outfile=<string>			Filename of the output image (default 'demo.pfm')." << endl << endl << \
	"Options for 'path' rendering algorithm:" << endl << \
	"	-s <value>, --seed=<value>			Random number generator seed (default 42)." << endl << \
	"	-n <value>, --nRays=<value>			Number of rays started at each intersection (default 3)." << endl << \
	"	-d <value>, --depth=<value>			Max ray depth (default 4)." << endl << \
	"	-r <value>, --roulette=<value>			Ray depth to start Russian roulette (default 3)." << endl

#define HELP_STACK \
	"stack: stack more pfm images representing the same scene." << endl << endl << \
	"Available options:" << endl << \
	"	-h, --help: print this message." << endl << \
	"	-m <string>, --method=<string>			The stacking method (can be 'mean' or 'median', default 'mean')." << endl << \
	"	-S <value>, --nSigma=<value>			Number of sigma clipping iterations (default 0)." << endl << \
	"	-a <value>, --alpha=<value>			Sigma clipping alpha factor (consider outliers values farther than alpha*sigma from the median, default 2)." << endl

#define HELP_RENDER \
	"render: render a scenefile to a pfm image." << endl << endl << \
	"Available options:" << endl << \
	"	-h, --help: print this message." << endl << \
	"	-f <variable1:value1,variable2:value2,...>, --float=<...>	Defines float variables to be used in the scenefile." << endl << \
	"	-o <string>, --outfile=<string>					Filename of the output image." << endl

using namespace std;

enum class ImageFormat { png, webp, jpeg, tiff, bmp, gif };

int demo(argh::parser cmdl);
int render(argh::parser cmdl);
int pfm2ldr(argh::parser cmdl);
int stackPfm(argh::parser cmdl);
string baseFilename(string s);

int main(int argc, char *argv[])
{
	argh::parser cmdl;

	cmdl.add_params({"-a", "--afactor", "--alpha", "--aspectRatio",
			 "-g", "--gamma",
			 "-c", "--compression",
			 "-q", "--quality",
			 "-l", "--luminosity",
			 "-w", "--width",
			 "-h", "--height",
			 "-p", "--projection",
			 "-D", "--angleDeg",
			 "-A", "--antialiasing",
			 "-n", "--nRays",
			 "-d", "--depth",
			 "-r", "--roulette",
			 "-R", "--renderer",
			 "-o", "--outfile",
			 "-s", "--seed",
			 "-f", "--float", "--format",
			 "-S", "--nSigma",
			 "-m", "--method"});
	cmdl.parse(argc, argv);

	const string programName = cmdl[0];
	const string actionName = cmdl[1];
	
	// Parse action
	if (actionName == "demo") { 
		return demo(cmdl);
	} else if (actionName == "render") {
		return render(cmdl);
	} else if (actionName == "pfm2ldr") {
		return pfm2ldr(cmdl);
	} else if (actionName == "stack") {
		return stackPfm(cmdl);
	} else if (cmdl[{"-h", "--help"}]) {
		cout << USAGE << endl << RUN_HELP;
		return 0;
	} else {
		cout << USAGE;
		return 1;
	}
}

int pfm2ldr(argh::parser cmdl)
{
	const string programName = cmdl[0];
	const string actionName = cmdl[1];

	if (cmdl[{"-h", "--help"}]) {
		cout << HELP_PFM2LDR;
		return 0;
	}

	if (cmdl.size() != 3) {
		cerr << USAGE;
		return 1;
	}

	// Parse input filename (first positional argument)
	string infileString = cmdl[2];
	const char *infile = infileString.c_str();

	// Parse format
	string formatStr;
	cmdl({"-f", "--format"}, "png") >> formatStr;
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

	// Parse output filename
	string ofileString;
	cmdl({"-o", "--outfile"}, baseFilename(infileString) + "." + formatStr) >> ofileString;
	const char *outfile = ofileString.c_str();

	// Parse parameters
	float aFactor;
	cmdl({"-a", "--afactor"}, .3f) >> aFactor;

	float gamma;
	cmdl({"-g", "--gamma"}, 1.f) >> gamma;

	bool palette = cmdl[{"-p", "--palette"}];

	int compression;
	cmdl({"-c", "--compression"}, -1) >> compression;

	int quality;
	cmdl({"-q", "--quality"}, -1) >> quality;

	float luminosity;
	cmdl({"-l", "--luminosity"}, -1.f) >> luminosity;

	// Read the input file
	HdrImage img;
	try {
		img.readPfm(infile);
	} catch (exception &e) {
		cerr << "Error: " <<  e.what() << endl;
		return 1;
	}

	// Convert HDR to LDR
	if (luminosity > 0.f)	// User inputted luminosity
		img.normalizeImage(aFactor, luminosity);
	else			// Default luminosity
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

int demo(argh::parser cmdl)
{
	const string programName = cmdl[0];
	const string actionName = cmdl[1];
	if (cmdl[{"-h", "--help"}]) {
		cout << HELP_DEMO;
		return 0;
	}

	int width, height;
	cmdl({"-w", "--width"}, 600) >> width;
	cmdl({"-h", "--height"}, 400) >> height;
	float aspectRatio;
	cmdl({"-a", "--aspectRatio"},  (float) width / height) >> aspectRatio;
	
	Material skyMat{DiffusiveBRDF{UniformPigment{WHITE}}, UniformPigment{WHITE}};
	Material wallsMat{DiffusiveBRDF{CheckeredPigment{Color{.8f, .8f, .8f}, Color{.2f, .2f, .2f}, 10}}};
	
	Material matteGreen{DiffusiveBRDF{UniformPigment{Color{.2f, .5f, .1f}}}};
	
	HdrImage marbleImage{"../textures/marble_10.pfm"};
	Material marble{DiffusiveBRDF{ImagePigment{marbleImage}}};

	HdrImage woodImage{"../textures/wood_20.pfm"};
	Material wood{DiffusiveBRDF{ImagePigment{woodImage}}};
	
	HdrImage noiseImage{"../textures/noise_10.pfm"};
	Material noise{DiffusiveBRDF{ImagePigment{noiseImage}}};

	int seed;
	cmdl({"-s", "--seed"}, 42) >> seed;
	float angle;
	cmdl({"-D", "--angleDeg"}, 0) >> angle;
	Transformation camTransformation{rotationZ(angle*M_PI/180)*translation(Vec{-1.f, 0.f, 0.f})};

	shared_ptr<Camera> cam;
	string projString;
	cmdl({"-p", "--projection"}, "perspective") >> projString;
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

	world.add(Sphere{translation(Vec{3.5f, 2.f, 0.f}), marble});
	world.add(Sphere{translation(Vec{3.5f, 0.f, 0.f}), wood});
	world.add(Sphere{translation(Vec{3.5f, -2.f, 0.f}), noise});
	world.add(Sphere{translation(Vec{1.5f, 0.f, -2.4f})*scaling(.5f), skyMat});


	world.add(Box{Point{-1.5f, -3.5f, -3.5f}, Point{5.5f, 3.5f, 3.5f}, wallsMat});
	world.add(Box{Point{-1.5f, -3.5f, 3.4f}, Point{5.5f, 3.5f, 3.5f}, skyMat});
	world.add(Box{Point{-1.5f, -3.5f, -3.5f}, Point{5.5f, 3.5f, -3.4f}, matteGreen});

	int samplesPerPixel;
	cmdl({"-A", "--antialiasing"}, 0) >> samplesPerPixel;
	int samplesPerSide = sqrt(samplesPerPixel);
	if (samplesPerPixel != samplesPerSide*samplesPerSide){
		cerr << "Not a perfect square given as --antialiasing parameter."  <<endl;
		return 1;
	}
	ImageTracer tracer{image, *cam, samplesPerSide};
	PCG pcg{(uint64_t) seed};

	int nRays;
	cmdl({"-n", "--nRays"}, 3) >> nRays;
	int depth;
	cmdl({"-d", "--depth"}, 4) >> depth;
	int roulette;
	cmdl({"-r", "--roulette"}, 3) >> roulette;

	string renderer;
	cmdl({"-R", "--renderer"}, "path") >> renderer;

	if (renderer == "path")
		tracer.fireAllRays(PathTracer{world, pcg, nRays, depth, roulette}, true);
	else if (renderer == "debug")
		tracer.fireAllRays(DebugRenderer{world});
	else if (renderer == "onoff")
		tracer.fireAllRays(OnOffRenderer{world});
	else if (renderer == "flat")
		tracer.fireAllRays(FlatRenderer{world});
	else {
		cerr << "Error: renderer " << renderer << " not supported" << endl;
		return 1;
	}

	string ofilename;
	cmdl({"-o", "--outfile"}, "demo.pfm") >> ofilename;
	ofstream outPfm;
	outPfm.open(ofilename);
	image.writePfm(outPfm);
	outPfm.close();

	return 0;

}

int render(argh::parser cmdl)
{
	const string programName = cmdl[0];
	const string actionName = cmdl[1];
	if (cmdl[{"-h", "--help"}]) {
		cerr << HELP_RENDER << endl;
		return 0;
	}

	if (cmdl.size() != 3) {
		cerr << USAGE << endl << HELP_RENDER;
		return 1;
	}

	int width, height;
	cmdl({"-w", "--width"}, 500) >> width;
	cmdl({"-h", "--height"}, 500) >> height;
	float aspectRatio = (float) width / height;

	int seed;
	cmdl({"-s", "--seed"}, 42) >> seed;

	int samplesPerPixel;
	cmdl({"--antialiasing"}, 0) >> samplesPerPixel;
	int samplesPerSide = sqrt(samplesPerPixel);
	if (samplesPerPixel != samplesPerSide*samplesPerSide){
		cerr << "Not a perfect square given as --antialiasing parameter."  <<endl;
		return 1;
	} 

	ifstream ifile{cmdl[2]};
	if (!ifile.is_open()) {
		cerr << "Cannot open file " + cmdl[2] << endl;
		return 1;
	}
	InputStream input{ifile, cmdl[2]};

	string variablesString;
	cmdl({"--float", "-f"}, string{}) >> variablesString;
	stringstream variablesStream{variablesString};
	variablesStream.peek();	// To set eofbit if the stream is empty.
	unordered_map<string, float> variables;
	while (!variablesStream.eof()) {
		string name;
		getline(variablesStream, name, ':');
		if (name.empty() or variablesStream.fail()) {
			cerr << "Error: expected variable name in --float definition" << endl;
			return 1;
		} else if (variablesStream.eof()) {
			cerr << "Error: expected : after variable name in --float definition" << endl;
			return 1;
		}
		float value;
		variablesStream >> value;
		if (variablesStream.fail()) {
			cerr << "Error: expected variable value after : in --float definition" << endl;
			return 1;
		}
		variables.insert({name, value});
		if (!variablesStream.eof() and variablesStream.get() != ',') {
			cerr << "Error: expected , or string end after value in --float definition" << endl;
			return 1;
		}
	}
	try {
		Scene scene{input.parseScene(variables, aspectRatio)};
		HdrImage image{width, height};
		ImageTracer tracer{image, *scene.camera, samplesPerSide};
		PCG pcg{(uint64_t) seed};
		tracer.fireAllRays(PathTracer{scene.world, pcg, 5, 5, 5});
		//tracer.fireAllRays(DebugRenderer(scene.world));

		string ofilename;
		cmdl({"-o", "--outfile"}, "render.pfm") >> ofilename;
		ofstream outPfm;
		outPfm.open(ofilename);
		image.writePfm(outPfm);
		outPfm.close();
	} catch (GrammarError &e) {
		cerr << "Error: " << string{e.location} << ": " << e.what() << endl;
		return 1;
	}

	return 0;
}

int stackPfm(argh::parser cmdl)
{
	const string programName = cmdl[0];
	const string actionName = cmdl[1];

	if (cmdl[{"-h", "--help"}]) {
		cout << HELP_STACK;
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

// Return the filename without the path and the extension
// E.g.: baseFilename("/usr/include/stdio.h") == "stdio".
string baseFilename(string s)
{
	s = s.substr(s.find_last_of('/')+1);
	s = s.substr(0, s.find_last_of('.'));
	return s;
}
