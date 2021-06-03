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
#include <iostream>
#include <iomanip>

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
			Material{DiffusiveBRDF{UniformPigment{WHITE}}}};
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

void testFlatRenderer()
{
	Color sphereColor{1.f, 2.f, 3.f};
	Sphere sphere{translation(Vec{2.f, 0.f, 0.f}) * scaling(0.2f, 0.2f, 0.2f),
			Material{DiffusiveBRDF{UniformPigment{sphereColor}}}};
	HdrImage image{3, 3};
	OrthogonalCamera camera;
	ImageTracer tracer{image, camera};
	World world;
	world.add(sphere);
	FlatRenderer renderer{world};
	tracer.fireAllRays(renderer);

	assert(image.getPixel(0, 0) == BLACK);
	assert(image.getPixel(1, 0) == BLACK);
	assert(image.getPixel(2, 0) == BLACK);
	assert(image.getPixel(0, 1) == BLACK);
	assert(image.getPixel(1, 1) == sphereColor);
	assert(image.getPixel(2, 1) == BLACK);
	assert(image.getPixel(0, 2) == BLACK);
	assert(image.getPixel(1, 2) == BLACK);
	assert(image.getPixel(2, 2) == BLACK);
}

// FAILING TEST
void testPathTracer()
{
	PCG pcg;
	for (int i{}; i<6; i++){
		World world;

		float emittedRadiance = pcg.randFloat();
		float reflectance = pcg.randFloat()*.8f; // 0.9f
		Material enclosureMat{
			DiffusiveBRDF{UniformPigment{Color{reflectance, reflectance, reflectance}}},
			UniformPigment{Color{emittedRadiance, emittedRadiance, emittedRadiance}}
		};
		world.add(Sphere{enclosureMat});
		PathTracer tracer{world, pcg, 1, 200, 201};
		Ray ray{Point{0.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
		Color color = tracer(ray);

		float expected = emittedRadiance / (1.f - reflectance);
		cout << i << '\t' << string(color) << '\t' << expected << endl;
		assert(areColorsClose(Color{expected, expected, expected}, color, 2.f)); //epsilon maybe better if smaller
	}
}


int main()
{
	testOnOffRenderer();
	testFlatRenderer();
	testPathTracer();
	return 0;
}
