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

#include "geometry.h"
#undef NDEBUG
#include <cassert>

using namespace std;

int main()
{
	float epsilon = 1e-10;
	Vec a{1.f, 2.f, 3.f};
	Vec b{4.f, 6.f, 8.f};

	assert(a == a);
	assert(!(a == b));
	assert(a != b);
	assert(areClose<Vec>(a, b, 10.f));

	assert((a + b) == (Vec{5.f, 8.f, 11.f}));
	assert((b - a) == (Vec{3.f, 4.f, 5.f}));
	assert((a * 2.f) == (Vec{2.f, 4.f, 6.f}));

	assert(_areClose(a.dot(b), 40.f, epsilon));
	assert(a.cross(b) == (Vec{-2.f, 4.f, -2.f}));
	assert(b.cross(a) == (Vec{2.f, -4.f, 2.f}));

	assert(_areClose(a.squaredNorm(), 14.f, epsilon));
	assert(_areClose(a.norm() * a.norm(), 14.f, epsilon));
	assert(a.versor() == (Vec{a.x/14.f, a.y/14.f, a.z/14.f}));
	a.normalize();
	assert(a == (Vec{a.x/14.f, a.y/14.f, a.z/14.f}));
    
	return 0;
}
