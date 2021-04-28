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

#include "color.h"
#undef NDEBUG
#include <cassert>

using namespace std;

int main()
{
	Color c1{1.0, 2.0, 3.0};
	Color c2{5.0, 7.0, 9.0};

	assert(c1==c1);
	assert(!(c1!=c1));
	assert(!(c1==c2));
	assert(c1!=c2);

	assert((c1+c2)==(Color{6.0, 9.0, 12.0}));
	
	assert((c1*1.0)==c1);
	assert((c1*5.0)!=c2);
	assert(!((c1*2.0)==c1));
	assert((c2*3.0)==(Color{15.0, 21.0, 27.0}));

	assert((c1*c2)==(Color{5.0, 14.0, 27.0}));

	Color c3{1e-11, 2e-11, 3e-11};
	Color c4{5e-11, 7e-11, 9e-11};

	assert(c3==c4);
	assert(!(c3!=c4));
	assert(!areColorsClose(c1, c3, 1e-12));

	Color c5{1.0, 2.0, 3.0};
	Color c6{9.0, 5.0, 7.0};

	assert(c5.luminosity() == 2.0);
	assert(c6.luminosity() == 7.0);

	return 0;
}
