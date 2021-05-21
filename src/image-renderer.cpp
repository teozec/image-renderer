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
#include "actions.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

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
	
	int exit = 0;
	// Parse action
	if (actionName == "demo") { 
		demo(cmdl, exit);
		return exit;
	} else if (actionName == "pfm2ldr") {
		pfm2ldr(cmdl, exit);
		return exit;
	} else if (cmdl[{"-h", "--help"}]) {
		cout << USAGE;
		return exit;
	} else {
		cout << USAGE << endl << RUN_HELP;
	}
}