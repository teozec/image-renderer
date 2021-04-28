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

#include "camera.h"
#include "geometry.h"
#undef NDEBUG
#include <cassert>
#include <cmath>

using namespace std;

void testImageTracer()
{
	HdrImage image{4, 2};
	Camera camera{2.f};
	ImageTracer tracer{image, camera};

	Ray ray1 = tracer.fireRay(0, 0, 2.5f, 1.5f);
	Ray ray2 = tracer.fireRay(2, 1);
	assert(ray1 == ray2);

	trace.fireAllRays([](Ray r) {return Color{1.f, 2.f, 3.f}});
	for (int row{}; row < image.height; row++)
		for (int col{}; col < image.width; col++)
			assert((image.getPixel(col, row) == Color{1.f, 2.f, 3.f}));
}

int main()
{
	// Test Ray
	Ray ray1{Point{1.f, 2.f, 3.f}, Vec{5.f, 4.f, -1.f}};
	Ray ray2{Point{1.f, 2.f, 3.f}, Vec{5.f, 4.f, -1.f}};
	Ray ray3{Point{5.f, 1.f, 4.f}, Vec{3.f, 9.f, 4.f}};
	assert(ray1 == ray2);
	assert(ray1 != ray3);

	Ray ray4{Point{1.f, 2.f, 4.f}, Vec{4.f, 2.f, 1.f}};
	assert(ray4(0.f) == ray4.origin);
	assert((ray4(1.f) == Point{5.f, 4.f, 5.f}));
	assert((ray4(2.f) == Point{9.f, 6.f, 6.f}));

	Ray ray5{Point{1.f, 2.f, 3.f}, Vec{6.f, 5.f, 4.f}};
	Transformation tr = translation(Vec{10.f, 11.f, 12.f}) * rotationX(M_PI/2);
	Ray transformed = tr * ray5;
	assert((transformed.origin == Point{11.f, 8.f, 14.f}));
	assert((transformed.dir == Vec{6.f, -4.f, 5.f}));

	testImageTracer()

	return 0;
}
