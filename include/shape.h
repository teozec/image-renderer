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

#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <memory>
#include <vector>
#include "geometry.h"
#include "camera.h"
#include <cmath>

struct Vec2D {
	float u, v;
	Vec2D() {}
	Vec2D(float u, float v): u{u}, v{v} {}

	Vec2D operator=(const Vec2D &other) {
		u = other.u;
		v = other.v;
		return *this;
	}
};

struct HitRecord {
	bool hit = false;
	Point worldPoint;
	Normal normal;
	Vec2D surfacePoint;
	float t;
	Ray ray;

	HitRecord() {}
	HitRecord(const HitRecord &other) :	//
		hit{other.hit}, worldPoint{other.worldPoint}, normal{other.normal}, //
		surfacePoint{other.surfacePoint}, t{other.t}, ray{other.ray} {}
	HitRecord(Point worldPoint, Normal normal, Vec2D surfacePoint, float t, Ray ray):
		worldPoint{worldPoint}, normal{normal}, surfacePoint{surfacePoint},
		t{t}, ray{ray}, hit{true} {}

	HitRecord operator=(const HitRecord &other) {
		hit = other.hit;
		worldPoint = other.worldPoint;
		normal = other.normal;
		surfacePoint = other.surfacePoint;
		t = other.t;
		ray = other.ray;
		return *this;
	}	
};

struct Shape {
	Transformation transformation;
	Shape(Transformation transformation = Transformation()): transformation{transformation} {}
	virtual HitRecord rayIntersection(Ray ray) = 0;
};

/**
 * @brief A Sphere object derived from Shape.
 * 
 * @param transformation	The transformation to be applied to the unit sphere centered at the origin.
 * @see Shape.
 */
struct Sphere : Shape {
	Sphere(): Shape() {}
	Sphere(Transformation transformation): Shape(transformation) {}

	HitRecord rayIntersection(Ray ray) {
		Ray invRay{transformation.inverse() * ray};
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		float delta4 = (origin.dot(dir)) * (origin.dot(dir)) -
			dir.squaredNorm() * (origin.squaredNorm() - 1.f);

		if (delta4 <= 0.f)
			return HitRecord{};

		float firstHit;
		float t1 = (-origin.dot(dir) - delta4) / dir.squaredNorm();
		float t2 = (-origin.dot(dir) + delta4) / dir.squaredNorm();
		if (invRay.tmin < t1 and t1 < invRay.tmax)
			firstHit = t1;
		else if (invRay.tmin < t2 and t2 < invRay.tmax)
			firstHit = t2;
		else
			return HitRecord{};

		Point hitPoint{invRay(firstHit)};
		return HitRecord{
			transformation * hitPoint,
			transformation * sphereNormal(hitPoint, ray.dir),
			spherePointToUV(hitPoint),
			firstHit,
			ray};
	}
	
private:
	Normal sphereNormal(Point p, Vec dir) {
		Normal result{p.x, p.y, p.z};
		return p.toVec().dot(dir) < 0. ? result : -result;
	}

	Vec2D spherePointToUV(Point p) {
		return Vec2D{std::atan2(p.x, p.y) / (float) (2. * M_PI), std::acos(p.z) / (float) M_PI};
	}
};

/** World class
 * @brief This is the class containing all the shapes of the scene.
 * 
 * @param shapes	List of shapes.
 */
struct World {
	std::vector<std::shared_ptr<Shape>> shapes;

	virtual void add(const Sphere &newShape){
		shapes.push_back(std::make_shared<Sphere>(newShape));
	}

	HitRecord rayIntersection(Ray ray){
		HitRecord closest{};
		for(int i{}; i<size(shapes); i++){
			HitRecord intersection = shapes[i]->rayIntersection(ray);
			if(!intersection.hit)
				continue;
			if((!closest.hit)||(intersection.t < closest.t))
				closest = intersection;
		}
		return closest;
	}
};

#endif // #SHAPE_H