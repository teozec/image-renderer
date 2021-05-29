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
#include <vector>

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

	vector<HitRecord> hit3 = sphere2.allIntersections(ray2);
	assert(hit3.size() == 2);
	assert((hit3[0].worldPoint == Point{1.f, 0.f, 0.f}));
	assert((hit3[0].normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit3[0].surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit3[0].t, 2.f));
	assert(hit3[0].ray == ray2);
	assert(hit3[1].t = 4.f);
	assert((hit3[1].worldPoint == Point{-1.f, 0.f, 0.f}));
	assert((hit3[1].normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit3[1].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit3[1].t, 4.f));
	assert(hit3[1].ray == ray2);

	Sphere sphere3;
	Ray ray3{Point{7.f, 0.f, 0.f}, Vec{-2.f, 0.f, 0.f}};
	HitRecord hit4{sphere3.rayIntersection(ray3)};
	assert(hit4.hit);
	assert((hit4.worldPoint == Point{1.f, 0.f, 0.f}));
	assert((hit4.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit4.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit4.t, 3.f));
	assert(hit4.ray == ray3);
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

	assert(sphere1.allIntersections(ray1).size() == 1);
	assert((sphere1.isInner(Point{0.f, 0.f, 0.f})));
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

	vector<HitRecord> hitList1 = sphere.allIntersections(ray1);
	assert(hitList1.size() == 2);
	assert((hitList1[0].worldPoint == Point{10.f, 0.f, 1.f}));
	assert((hitList1[0].normal == Normal{0.f, 0.f, 1.f}));
	assert((areSphereSurfacePointsClose(hitList1[0].surfacePoint, Vec2D{0.f, 0.f})));
	assert(areClose(hitList1[0].t, 1.f));
	assert(hitList1[0].ray == ray1);
	assert((hitList1[1].worldPoint == Point{10.f, 0.f, -1.f}));
	assert((hitList1[1].normal == Normal{0.f, 0.f, 1.f}));
	assert((areSphereSurfacePointsClose(hitList1[1].surfacePoint, Vec2D{1.f, 0.f})));
	assert(areClose(hitList1[1].t, 3.f));
	assert(hitList1[1].ray == ray1);

	Ray ray2{Point{13.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};
	HitRecord hit2{sphere.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{11.f, 0.f, 0.f}));
	assert((hit2.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit2.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);

	vector<HitRecord> hitList2 = sphere.allIntersections(ray2);
	assert(hitList2.size() == 2);
	assert((hitList2[0].worldPoint == Point{11.f, 0.f, 0.f}));
	assert((hitList2[0].normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList2[0].surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hitList2[0].t, 2.f));
	assert(hitList2[0].ray == ray2);
	assert((hitList2[1].worldPoint == Point{9.f, 0.f, 0.f}));
	assert((hitList2[1].normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList2[1].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hitList2[1].t, 4.f));
	assert(hitList2[1].ray == ray2);

	Ray ray3{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit3{sphere.rayIntersection(ray3)};
	assert(!hit3.hit);
	Ray ray4{Point{-10.f, 0.f, 0.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit4{sphere.rayIntersection(ray4)};
	assert(!hit4.hit);
	assert(sphere.allIntersections(ray3).size() == 0);

	assert((sphere.isInner(Point{10.f, 0.f, 0.f})));
	assert(!(sphere.isInner(Point{0.f, 0.f, 0.f})));
}

void testPlane()
{
	Plane plane;
	Ray ray1{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit1{plane.rayIntersection(ray1)};
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{0.f, 0.f, 0.f}));
	assert((hit1.normal == Normal{0.f, 0.f, 1.f}));
	assert(areClose(hit1.t, 2.f));
	assert(hit1.ray == ray1);

	Ray ray2{Point{0.f, 2.f, -2.f}, Vec{0.f, 0.f, 1.f}};
	HitRecord hit2{plane.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{0.f, 2.f, 0.f}));
	assert((hit2.normal == Normal{0.f, 0.f, -1.f}));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);

	assert((plane.isInner(Point{1.f, 2.f, -3.f})));
	assert(!(plane.isInner(Point{-1.f, -2.f, 3.f})));

	Ray ray3{Point{0.f, 2.f, -2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit3{plane.rayIntersection(ray3)};
	assert(!hit3.hit);
}

void testPlaneTransformation()
{
	Plane plane{rotationY(M_PI_2)};
	Ray ray2{Point{2.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};
	HitRecord hit2{plane.rayIntersection(ray2)};
	assert(hit2.hit);
	assert((hit2.worldPoint == Point{0.f, 0.f, 0.f}));
	assert(hit2.normal==(Normal{1.f, 0.f, 0.f}));
	assert(areClose(hit2.t, 2.f));
	assert(hit2.ray == ray2);

	assert((plane.isInner(Point{-1.f, 2.f, 3.f})));
	assert(!(plane.isInner(Point{1.f, 2.f, 3.f})));
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

	// Test rayIntersection form both sides without transformation
	CSGUnion union1{sphere1, sphere2};
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

	// Test isInner without transformation
	assert((union1.isInner(Point{0.f, 0.f, 0.f})));
	assert((union1.isInner(Point{-1.f, 0.f, 0.f})));
	assert((union1.isInner(Point{1.f, 0.f, 0.f})));

	// Test rayIntersection form both sides with transformation
	CSGUnion union2{sphere1, sphere2, translation(Vec{0.f, 10.f, 0.f})};
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

	// Test isInner with transformation
	assert((union2.isInner(Point{0.f, 10.f, 0.f})));
	assert((union2.isInner(Point{-1.f, 10.f, 0.f})));
	assert((union2.isInner(Point{1.f, 10.f, 0.f})));
	assert(!(union2.isInner(Point{0.f, 0.f, 0.f})));

	// Test rayIntersection starting from inner point, without transformation
	Ray ray5{Point{0.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	HitRecord hit6 = union1.rayIntersection(ray5);
	assert(hit6.hit);
	assert(hit6.worldPoint == (Point{.5f, 0.f, 0.f}));
	assert((hit6.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit6.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit6.t, .5f));
	assert(hit6.ray == ray5);

	// Test allIntersections with transformation
	Ray ray7{Point{-2.5f, 10.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	vector<HitRecord> hitList{union2.allIntersections(ray7)};
	assert(hitList.size() == 4);
	assert(hitList[0].worldPoint == (Point{-1.5f, 10.f, 0.f}));
	assert((hitList[0].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[0].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hitList[0].t, 1.f));
	assert(hitList[0].ray == ray7);
	assert(hitList[1].worldPoint == (Point{-.5f, 10.f, 0.f}));
	assert((hitList[1].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[1].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hitList[1].t, 2.f));
	assert(hitList[1].ray == ray7);
	assert(hitList[2].worldPoint == (Point{.5f, 10.f, 0.f}));
	assert((hitList[2].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[2].surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hitList[2].t, 3.f));
	assert(hitList[2].ray == ray7);
	assert(hitList[3].worldPoint == (Point{1.5f, 10.f, 0.f}));
	assert((hitList[3].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[3].surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hitList[3].t, 4.f));
	assert(hitList[3].ray == ray7);

	// Test rayIntersection with only one of the two shapes, without transformation
	Ray ray8{Point{-1.f, -2.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	assert(union1.rayIntersection(ray8).hit);
	Ray ray9{Point{1.f, -2.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	assert(union1.rayIntersection(ray9).hit);
}

void testCSGDifference()
{
	Sphere sphere1{translation(Vec{-.5f, 0.f, 0.f})};
	Sphere sphere2{translation(Vec{.5f, 0.f, 0.f})};

	// Test rayIntersection form both sides without transformation
	CSGDifference diff1{sphere1, sphere2};
	Ray ray1{Point{-2.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	Ray ray2{Point{2.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};

	HitRecord hit1 = diff1.rayIntersection(ray1);
	assert(hit1.hit);
	assert(hit1.worldPoint == (Point{-1.5f, 0.f, 0.f}));
	assert((hit1.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit1.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit1.t, .5f));
	assert(hit1.ray == ray1);

	HitRecord hit2 = diff1.rayIntersection(ray2);
	assert(hit2.hit);
	assert(hit2.worldPoint == (Point{-0.5f, 0.f, 0.f}));
	assert((hit2.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit2.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit2.t, 2.5f));
	assert(hit2.ray == ray2);

	// Test isInner without transformation
	assert(!(diff1.isInner(Point{0.f, 0.f, 0.f})));
	assert((diff1.isInner(Point{-1.f, 0.f, 0.f})));
	assert(!(diff1.isInner(Point{1.f, 0.f, 0.f})));

	// Test rayIntersection form both sides with transformation
	CSGDifference diff2{sphere1, sphere2, translation(Vec{0.f, 10.f, 0.f})};
	Ray ray3{Point{-2.25f, 10.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	Ray ray4{Point{1.75, 10.f, 0.f}, Vec{-1.f, 0.f, 0.f}};

	HitRecord hit3 = diff2.rayIntersection(ray1);
	assert(!hit3.hit);

	HitRecord hit4 = diff2.rayIntersection(ray3);
	assert(hit4.hit);
	assert(hit4.worldPoint == (Point{-1.5f, 10.f, 0.f}));
	assert((hit4.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit4.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit4.t, .75f));
	assert(hit4.ray == ray3);

	HitRecord hit5 = diff2.rayIntersection(ray4);
	assert(hit5.hit);
	assert(hit5.worldPoint == (Point{-0.5f, 10.f, 0.f}));
	assert((hit5.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit5.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit5.t, 2.25f));
	assert(hit5.ray == ray4);

	// Test isInner with transformation
	assert(!(diff2.isInner(Point{0.f, 10.f, 0.f})));
	assert((diff2.isInner(Point{-1.f, 10.f, 0.f})));
	assert(!(diff2.isInner(Point{1.f, 10.f, 0.f})));
	assert(!(diff2.isInner(Point{-1.f, 0.f, 0.f})));

	// Test rayIntersection starting from inner point, without transformation
	Ray ray5{Point{-1.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	HitRecord hit6 = diff1.rayIntersection(ray5);
	assert(hit6.hit);
	assert(hit6.worldPoint == (Point{-.5f, 0.f, 0.f}));
	assert((hit6.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit6.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit6.t, .5f));
	assert(hit6.ray == ray5);

	// Test allIntersections with transformation
	Ray ray7{Point{-2.5f, 10.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	vector<HitRecord> hitList{diff2.allIntersections(ray7)};
	assert(hitList.size() == 2);
	assert(hitList[0].worldPoint == (Point{-1.5f, 10.f, 0.f}));
	assert((hitList[0].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[0].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hitList[0].t, 1.f));
	assert(hitList[0].ray == ray7);
	assert(hitList[1].worldPoint == (Point{-.5f, 10.f, 0.f}));
	assert((hitList[1].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[1].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hitList[1].t, 2.f));
	assert(hitList[1].ray == ray7);

	// Test rayIntersection with only one of the two shapes, without transformation
	Ray ray8{Point{-1.f, -2.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	assert(diff1.rayIntersection(ray8).hit);
	Ray ray9{Point{1.f, -2.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	assert(!diff1.rayIntersection(ray9).hit);
}

void testCSGIntersection()
{
	Sphere sphere1{translation(Vec{-.5f, 0.f, 0.f})};
	Sphere sphere2{translation(Vec{.5f, 0.f, 0.f})};

	// Test rayIntersection form both sides without transformation
	CSGIntersection inters1{sphere1, sphere2};
	Ray ray1{Point{-2.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	Ray ray2{Point{2.f, 0.f, 0.f}, Vec{-1.f, 0.f, 0.f}};

	HitRecord hit1 = inters1.rayIntersection(ray1);
	assert(hit1.hit);
	assert(hit1.worldPoint == (Point{-.5f, 0.f, 0.f}));
	assert((hit1.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit1.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit1.t, 1.5f));
	assert(hit1.ray == ray1);

	HitRecord hit2 = inters1.rayIntersection(ray2);
	assert(hit2.hit);
	assert(hit2.worldPoint == (Point{.5f, 0.f, 0.f}));
	assert((hit2.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit2.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit2.t, 1.5f));
	assert(hit2.ray == ray2);

	// Test isInner without transformation
	assert((inters1.isInner(Point{0.f, 0.f, 0.f})));
	assert(!(inters1.isInner(Point{-1.f, 0.f, 0.f})));
	assert(!(inters1.isInner(Point{1.f, 0.f, 0.f})));

	// Test rayIntersection form both sides with transformation
	CSGIntersection inters2{sphere1, sphere2, translation(Vec{0.f, 10.f, 0.f})};
	Ray ray3{Point{-2.25f, 10.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	Ray ray4{Point{1.75, 10.f, 0.f}, Vec{-1.f, 0.f, 0.f}};

	HitRecord hit3 = inters2.rayIntersection(ray1);
	assert(!hit3.hit);

	HitRecord hit4 = inters2.rayIntersection(ray3);
	assert(hit4.hit);
	assert(hit4.worldPoint == (Point{-0.5f, 10.f, 0.f}));
	assert((hit4.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit4.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit4.t, 1.75f));
	assert(hit4.ray == ray3);

	HitRecord hit5 = inters2.rayIntersection(ray4);
	assert(hit5.hit);
	assert(hit5.worldPoint == (Point{0.5f, 10.f, 0.f}));
	assert((hit5.normal == Normal{1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit5.surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hit5.t, 1.25f));
	assert(hit5.ray == ray4);

	// Test isInner with transformation
	assert((inters2.isInner(Point{0.f, 10.f, 0.f})));
	assert(!(inters2.isInner(Point{-1.f, 10.f, 0.f})));
	assert(!(inters2.isInner(Point{1.f, 10.f, 0.f})));
	assert(!(inters2.isInner(Point{0.f, 0.f, 0.f})));

	// Test rayIntersection starting from inner point, without transformation
	Ray ray5{Point{-1.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	HitRecord hit6 = inters1.rayIntersection(ray5);
	assert(hit6.hit);
	assert(hit6.worldPoint == (Point{-.5f, 0.f, 0.f}));
	assert((hit6.normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hit6.surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hit6.t, .5f));
	assert(hit6.ray == ray5);

	// Test allIntersections with transformation
	Ray ray7{Point{-2.5f, 10.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	vector<HitRecord> hitList{inters2.allIntersections(ray7)};
	assert(hitList.size() == 2);
	assert(hitList[0].worldPoint == (Point{-.5f, 10.f, 0.f}));
	assert((hitList[0].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[0].surfacePoint, Vec2D{0.5f, 0.5f})));
	assert(areClose(hitList[0].t, 2.f));
	assert(hitList[0].ray == ray7);
	assert(hitList[1].worldPoint == (Point{.5f, 10.f, 0.f}));
	assert((hitList[1].normal == Normal{-1.f, 0.f, 0.f}));
	assert((areSphereSurfacePointsClose(hitList[1].surfacePoint, Vec2D{0.5f, 0.f})));
	assert(areClose(hitList[1].t, 3.f));
	assert(hitList[1].ray == ray7);

	// Test rayIntersection with only one of the two shapes, without transformation
	Ray ray8{Point{-1.f, -2.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	assert(!inters1.rayIntersection(ray8).hit);
	Ray ray9{Point{1.f, -2.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	assert(!inters1.rayIntersection(ray9).hit);
}


void testTriangle() {
	Triangle triangle{Point{1.f, 1.f, 0.f}, Point{0.f, 1.f, 0.f}, Point{1.f, 0.f, 0.f}};
	Ray ray{Point{(float)2/3, (float)2/3, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord hit{triangle.rayIntersection(ray)};
	assert(hit.hit);
	assert(hit.worldPoint == (Point{(float)2/3, (float)2/3, 0.f}));
	assert(hit.normal==(Normal{0.f, 0.f, 1.f}));
	assert(hit.t == 2.f);
	assert(hit.ray == ray);
	Ray extRay{Point{(float)1/3, (float)1/3, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord extHit{triangle.rayIntersection(extRay)};
	assert(!(extHit.hit));
}

void testTriangleTransformation() {
	Triangle triangle{rotationX(M_PI_2)};
	Ray ray{Point{2.f, -(float)1/3, (float)1/3}, Vec{-1.f, 0.f, 0.f}};
	HitRecord hit{triangle.rayIntersection(ray)};
	assert(hit.hit);
	assert(hit.worldPoint == (Point{0.f, -(float)1/3, (float)1/3}));
	assert(hit.normal==(Normal{1.f, 0.f, 0.f}));
	assert(areClose(hit.t, 2.f));
	assert(hit.ray == ray);
	Ray extRay{Point{(float)2/3, (float)2/3, 2.f}, Vec{0.f, 0.f, -1.f}};
	HitRecord extHit{triangle.rayIntersection(extRay)};
	assert(!(extHit.hit));
}

void testBox()
{
	Box box{Point{-1.f, -2.f, -3.f}, Point{4.f, 5.f, 6.f}};

	Ray ray1{Point{-4.f, 0.f, 0.f}, Vec{1.f, 0.f, 0.f}};
	HitRecord hit1{box.rayIntersection(ray1)};
	assert(hit1.hit);
	assert(hit1.worldPoint == (Point{-1.f, 0.f, 0.f}));
	assert(hit1.normal==(Normal{-1.f, 0.f, 0.f}));
	assert(hit1.surfacePoint == (Vec2D{(2.f / 7.f) / 6.f, (3.f / 9.f) / 6.f}));
	assert(areClose(hit1.t, 3.f));
	assert(hit1.ray == ray1);

	vector<HitRecord> hitAll1{box.allIntersections(ray1)};
	assert(hitAll1.size() == 2);
	assert(hitAll1[0].worldPoint == (Point{-1.f, 0.f, 0.f}));
	assert(hitAll1[0].normal==(Normal{-1.f, 0.f, 0.f}));
	assert(hitAll1[0].surfacePoint == (Vec2D{(2.f / 7.f) / 6.f, (3.f / 9.f) / 6.f}));
	assert(areClose(hitAll1[0].t, 3.f));
	assert(hitAll1[0].ray == ray1);
	assert(hitAll1[1].worldPoint == (Point{4.f, 0.f, 0.f}));
	assert(hitAll1[1].normal==(Normal{-1.f, 0.f, 0.f}));
	assert(hitAll1[1].surfacePoint == (Vec2D{(3.f + 2.f / 7.f) / 6.f, (3.f + 3.f / 9.f) / 6.f}));
	assert(areClose(hitAll1[1].t, 8.f));
	assert(hitAll1[1].ray == ray1);

	Ray ray2{Point{-4.f, 0.f, 0.f}, Vec{0.f, 1.f, 0.f}};
	cout << string(box.rayIntersection(ray2).worldPoint) << endl;
	assert(!box.rayIntersection(ray2).hit);
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
	testCSGDifference();
	testCSGIntersection();
	testTriangle();
	testTriangleTransformation();
	testBox();
	return 0;
}
