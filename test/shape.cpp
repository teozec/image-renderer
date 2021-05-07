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
#include "shape.h"
#include "geometry.h"
#undef NDEBUG
#include <cassert>
#include <iostream>
#include <cmath>

using namespace std;

bool areClose(float x, float y)
{
	const float epsilon = 1e-5;
	return abs(x-y) < epsilon;
}

bool areSphereSurfacePointsClose(Vec2D a, Vec2D b)
{
	// Theta
	if (!areClose(a.u, b.u))
		return false;
	// Phi is defined only if theta != 0 or theta != pi (1 in normalized coordinates)
	if (!areClose(a.u, 0.f) and !areClose(a.u, 1.f))
		if (!areClose(a.v, b.v))
			return false;
	return true;
}

void testSphere()
{
	Sphere sphere1;
	Ray ray1{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit1{sphere1.rayIntersection(ray1)};
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{0.f, 0.f, 1.f}));
	assert((hit1.normal == Normal{0.f, 0.f, 1.f}));
	assert((areSphereSurfacePointsClose(hit1.surfacePoint, Vec2D{0.f, 0.f})));
	assert(areClose(hit1.t, 1.f));
	assert(hit1.ray == ray1);

	Sphere sphere2;
	Ray ray2{Point{3.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};
	HitRecord hit2{sphere2.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{1.f, 0.f, 0.f}));
	assert((hit2.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit2.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);
}

void testSphereInner()
{
	Sphere sphere1;
	Ray ray1{Point{0.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	HitRecord hit1{sphere1.rayIntersection(ray1)};
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{1.f, 0.f, 0.f}));
	assert((hit1.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit1.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit1.t, 1.f));
	assert(hit1.ray == ray1);
}

void testSphereTransformation()
{
	Sphere sphere{translation(Vec{10.f, 0.f, 0.f})};

	Ray ray1{Point{10.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit1{sphere.rayIntersection(ray1)};
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{10.f, 0.f, 1.f}));
	assert((hit1.normal == Normal{0.f, 0.f, 1.f}));
	assert((areSphereSurfacePointsClose(hit1.surfacePoint, Vec2D{0.f, 0.f})));
	assert(areClose(hit1.t, 1.f));
	assert(hit1.ray == ray1);

	Ray ray2{Point{13.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};
	HitRecord hit2{sphere.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{11.f, 0.f, 0.f}));
	assert((hit2.normal == Normal{1.f, 0.f, 0.f}));
	cout << hit2.surfacePoint.u << '\t' << hit2.surfacePoint.v << endl;
	assert((areSphereSurfacePointsClose(hit2.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);

	Ray ray3{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit3{sphere.rayIntersection(ray3)};
	assert(!hit3.hit);
	Ray ray4{Point{-10.f, 0.f, 0.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit4{sphere.rayIntersection(ray4)};
	assert(!hit4.hit);
}

int main()
{
	testSphere();
	testSphereInner();
	testSphereTransformation();
	return 0;
}
