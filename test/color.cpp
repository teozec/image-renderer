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
	Color c1{1.f, 2.f, 3.f};
	Color c2{5.f, 7.f, 9.f};

	assert(c1==c1);
	assert(!(c1!=c1));
	assert(!(c1==c2));
	assert(c1!=c2);

	assert((c1+c2)==(Color{6.f, 9.f, 12.f}));
	
	assert((c1*1.f)==c1);
	assert((c1*5.f)!=c2);
	assert(!((c1*2.f)==c1));
	assert((c2*3.f)==(Color{15.f, 21.f, 27.f}));

	assert((c1/1.f)==c1);
	assert((c1/5.f)!=c2);
	assert(!((c1/2.f)==c1));
	assert((c2/2.f)==(Color{2.5f, 3.5f, 4.5f}));

	assert((c1*c2)==(Color{5.f, 14.f, 27.f}));

	c1 *= 2.f;
	assert((c1 == Color{2.f, 4.f, 6.f}));

	c2 /= 2.f;
	assert((c2==Color{2.5f, 3.5f, 4.5f}));

	Color c3{1e-11f, 2e-11f, 3e-11f};
	Color c4{5e-11f, 7e-11f, 9e-11f};

	assert(c3==c4);
	assert(!(c3!=c4));
	assert(!c1.isClose(c3, 1e-12f));

	Color c5{1.f, 2.f, 3.f};
	Color c6{9.f, 5.f, 7.f};

	assert(c5.luminosity() == 2.f);
	assert(c6.luminosity() == 7.f);

	return 0;
}
