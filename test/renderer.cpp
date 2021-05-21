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

#include <memory>

#include "hdr-image.h"
#include "renderer.h"
#include "shape.h"
#include "geometry.h"
#include "material.h"
#include "color.h"

#undef NDEBUG
#include <cassert>

using namespace std;

void testOnOffRenderer()
{
	Sphere sphere{translation(Vec{2.f, 0.f, 0.f}) * scaling(0.2f, 0.2f, 0.2f),
			Material{make_shared<DiffusiveBRDF>(DiffusiveBRDF{make_shared<UniformPigment>(UniformPigment{WHITE})})}};
	HdrImage image{3, 3};
	OrthogonalCamera camera;
	ImageTracer tracer{image, camera};
	World world;
	world.add(sphere);
	OnOffRenderer renderer{world};
	tracer.fireAllRays(renderer);

	assert(image.getPixel(0, 0) == BLACK);
	assert(image.getPixel(1, 0) == BLACK);
	assert(image.getPixel(2, 0) == BLACK);
	assert(image.getPixel(0, 1) == BLACK);
	assert(image.getPixel(1, 1) == WHITE);
	assert(image.getPixel(2, 1) == BLACK);
	assert(image.getPixel(0, 2) == BLACK);
	assert(image.getPixel(1, 2) == BLACK);
	assert(image.getPixel(2, 2) == BLACK);
}

int main()
{
	testOnOffRenderer();
	return 0;
}
