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

void testPlane()
{
	Plane plane1;
	Ray ray1{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit1{plane1.rayIntersection(ray1)};
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{0.f, 0.f, 0.f}));
	assert((hit1.normal == Normal{0.f, 0.f, 1.f}));
	assert(areClose(hit1.t, 2.f));
	assert(hit1.ray == ray1);

	Ray ray2{Point{0.f, 2.f, -2.f}, Vec{0.f, 0.f, 1.f}};
	HitRecord hit2{plane1.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{0.f, 2.f, 0.f}));
	assert((hit2.normal == Normal{0.f, 0.f, -1.f}));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);
}

void testPlaneTransformation()
{
	Plane plane2{rotationY(M_PI_2)};
	Ray ray2{Point{2.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};
	HitRecord hit2{plane2.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{0.f, 0.f, 0.f}));
	assert(hit2.normal==(Normal{-1.f, 0.f, 0.f}));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);
}

void testWorld()
{
	World world;
	Sphere sphere1{translation(Vec{2.f, 0.f, 0.f})};
	Sphere sphere2{translation(Vec{8.f, 0.f, 0.f})};
	world.add(sphere1);
	world.add(sphere2);
	HitRecord intersection1 = world.rayIntersection(Ray{Point{0.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}});
	assert(intersection1.hit);
	assert(intersection1.worldPoint == (Point{1.f, 0.f, 0.f}));
	HitRecord intersection2 = world.rayIntersection(Ray{Point{10.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}});
	assert(intersection2.hit);
	assert(intersection2.worldPoint == (Point{9.f, 0.f, 0.f}));
}

void testCSGUnion()
{
	Sphere sphere1{translation(Vec{-.5f, 0.f, 0.f})};
	Sphere sphere2{translation(Vec{.5f, 0.f, 0.f})};

	CSGUnion union1{make_shared<Sphere>(sphere1), make_shared<Sphere>(sphere2)};
	Ray ray1{Point{-2.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	Ray ray2{Point{2.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};

	HitRecord hit1 = union1.rayIntersection(ray1);
	assert(hit1.hit);
	assert(hit1.worldPoint == (Point{-1.5f, 0.f, 0.f}));
	assert((hit1.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit1.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit1.t, .5f));
	assert(hit1.ray == ray1);

	HitRecord hit2 = union1.rayIntersection(ray2);
	assert(hit2.hit);
	assert(hit2.worldPoint == (Point{1.5f, 0.f, 0.f}));
	assert((hit2.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit2.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit2.t, .5f));
	assert(hit2.ray == ray2);

	CSGUnion union2{make_shared<Sphere>(sphere1), make_shared<Sphere>(sphere2), translation(Vec{0.f, 10.f, 0.f})};
	Ray ray3{Point{-2.25f, 10.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	Ray ray4{Point{1.75, 10.f, 0.f}, Vec{-1.f, 0.f, 0.f}};

	HitRecord hit3 = union2.rayIntersection(ray1);
	assert(!hit3.hit);

	HitRecord hit4 = union2.rayIntersection(ray3);
	assert(hit4.hit);
	assert(hit4.worldPoint == (Point{-1.5f, 10.f, 0.f}));
	assert((hit4.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit4.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit4.t, .75f));
	assert(hit4.ray == ray3);

	HitRecord hit5 = union2.rayIntersection(ray4);
	assert(hit5.hit);
	assert(hit5.worldPoint == (Point{1.5f, 10.f, 0.f}));
	assert((hit5.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit5.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit5.t, .25f));
	assert(hit5.ray == ray4);

	Ray ray5{Point{0.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	HitRecord hit6 = union1.rayIntersection(ray5);
	assert(hit6.hit);
	assert(hit6.worldPoint == (Point{.5f, 0.f, 0.f}));
	assert((hit6.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit6.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit6.t, .5f));
	assert(hit6.ray == ray5);
}

int main()
{
	testSphere();
	testSphereInner();
	testSphereTransformation();
	testPlane();
	testPlaneTransformation();
	testWorld();
	testCSGUnion();

	return 0;
}
