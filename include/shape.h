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
#include <algorithm>
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
		hit{true}, worldPoint{worldPoint}, normal{normal}, surfacePoint{surfacePoint},
		t{t}, ray{ray} {}

	HitRecord operator=(const HitRecord &other) {
		hit = other.hit;
		if (hit) {
			worldPoint = other.worldPoint;
			normal = other.normal;
			surfacePoint = other.surfacePoint;
			t = other.t;
			ray = other.ray;
		}
		return *this;
	}	

	bool operator<(const HitRecord &other) const {
		return this->t < other.t;
	}

	bool operator>(const HitRecord &other) const {
		return this->t > other.t;
	}
};

/**
 * @brief A Shape abstract struct.
 *
 * @param transformation	The transformation to be applied to the shape.
 */
struct Shape {
	Transformation transformation;
	Shape(Transformation transformation = Transformation()): transformation{transformation} {}

	/**
	 * @brief Return a HitRecord corresponding to the first intersection between the shape and the ray.
	 */
	virtual HitRecord rayIntersection(Ray ray) = 0;

	/**
	 * @brief Return a vector of HitRecord corresponding to all the intersections, ordered by increasing t.
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) = 0;

	virtual bool isInner(Point p) = 0;
};

/**
 * @brief A Sphere object derived from Shape.
 * 
 * @param transformation	The transformation to be applied to the unit sphere centered at the origin.
 * @see Shape.
 */
struct Sphere : public Shape {
	Sphere(): Shape() {}
	Sphere(Transformation transformation): Shape(transformation) {}

	virtual HitRecord rayIntersection(Ray ray) override {
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
			return intersection(t1, ray, invRay);
		else if (invRay.tmin < t2 and t2 < invRay.tmax)
			return intersection(t2, ray, invRay);
		else
			return HitRecord{};
	}

	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		std::vector<HitRecord> intersections;

		float delta4 = (origin.dot(dir)) * (origin.dot(dir)) -
			dir.squaredNorm() * (origin.squaredNorm() - 1.f);

		if (delta4 <= 0.f)
			return intersections;

		float t1 = (-origin.dot(dir) - delta4) / dir.squaredNorm();
		float t2 = (-origin.dot(dir) + delta4) / dir.squaredNorm();
		if (invRay.tmin < t1 and t1 < invRay.tmax)
			intersections.push_back(intersection(t1, ray, invRay));
		if (invRay.tmin < t2 and t2 < invRay.tmax)
			intersections.push_back(intersection(t2, ray, invRay));

		return intersections;
	}

	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return p.x * p.x + p.y * p.y + p.z * p.z < 1.f;
	}

private:
	HitRecord intersection(float t, Ray ray, Ray invRay) {
		Point hitPoint{invRay(t)};
		return HitRecord{
			transformation * hitPoint,
			transformation * sphereNormal(hitPoint, ray.dir),
			spherePointToUV(hitPoint),
			t,
			ray};
	}

	Normal sphereNormal(Point p, Vec dir) {
		Normal result{p.x, p.y, p.z};
		return p.toVec().dot(dir) < 0. ? result : -result;
	}

	Vec2D spherePointToUV(Point p) {
		return Vec2D{std::acos(p.z) / (float) M_PI, std::atan2(p.y, p.x) / (float) (2. * M_PI)};
	}
};

/**
 * @brief A Plane object derived from Shape.
 * 
 * @param transformation	The transformation to the xy plane.
 * @see Shape.
 */
struct Plane : public Shape {
	Plane(): Shape() {}
	Plane(Transformation transformation): Shape(transformation) {}

	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		const float epsilon = 1e-5;

		if (std::abs(dir.z - 0.f) < epsilon)
			return HitRecord{};
		float t = -origin.z / dir.z;

		Point hitPoint{invRay(t)};
		return HitRecord{
			transformation * hitPoint,
			transformation * planeNormal(hitPoint, ray.dir),
			planePointToUV(hitPoint),
			t,
			ray};
	}

	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		HitRecord hit = rayIntersection(ray);
		if (hit.hit)
			return std::vector<HitRecord>{hit};
		else
			return std::vector<HitRecord>{};
	}

	// By convention, the plane inner part is the z<0 half space
	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return p.z < 0;
	}

private:
	Normal planeNormal(Point p, Vec dir) {
		Normal result{0.f, 0.f, 1.f};
		return dir.z < 0. ? result : -result;
	}

	Vec2D planePointToUV(Point p) {
		return Vec2D{p.x - std::floor(p.x), p.y - std::floor(p.y)};
	}
};

struct Triangle : public Shape {

	Triangle(): Shape() {}
	Triangle(Transformation transform): Shape(transform) {
		A = transform*(this->A);
		B = transform*(this->B);
		C = transform*(this->C);
		} 
	Triangle(Point a, Point b, Point c, Transformation transform = Transformation{}) : Shape(transform) {
		A = transform*a;
		B = transform*b;
		C = transform*c;
	}

	virtual HitRecord rayIntersection(Ray ray) override {
		float s[3][3] = {{(B-A).x, (C-A).x, ray.dir.x},
						{(B-A).y, (C-A).y, ray.dir.y},
						{(B-A).z, (C-A).z, ray.dir.z}};
		Vec b{ray.origin-A};
		float beta, gamma, t;

		if (determinantOfMatrix(s) == 0.f)
			return HitRecord{};

		std::vector<float> solution = findSolution(s, b);
		if (!(ray.tmin < solution[2] && solution[2] < ray.tmax) 
			|| !(0 < solution[1] && solution[1] < 1)
			|| !(0 < solution[0] && solution[0] < 1)
			|| !(0 < 1-solution[0]-solution[1] && 1-solution[0]-solution[1] < 1)) {
			return HitRecord{};
		}

		t = solution[2];
		gamma = solution[1];
		beta = solution[0];
		Point hitPoint = ray(t);
		Vec perp = (B-A).cross(C-A);
		Normal hitNormal = triangleNormal(perp, ray.dir);
		return HitRecord{
			hitPoint,
			hitNormal,
			trianglePointToUV(beta, gamma),
			t,
			ray};
	}

	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		HitRecord hit = rayIntersection(ray);
		if (hit.hit)
			return std::vector<HitRecord>{hit};
		else
			return std::vector<HitRecord>{};
	}

	// Not implemented
	virtual bool isInner(Point p) override {
		return false;
	}

private:

	Point A{0.f, 0.f, 0.f}, B{0.f, 1.f, 0.f}, C{0.f, 0.f, 1.f};

  	Normal triangleNormal(Vec p, Vec dir) {
		p.normalize();
		Normal result{p.x, p.y, p.z};
		return dir.dot(p) < 0. ? result : -result;
	}

	Vec2D trianglePointToUV(float beta, float gamma) {
		return Vec2D{beta, gamma};
	}

	float determinantOfMatrix(float mat[3][3]) {
		float ans;
		ans = mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2])
			- mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
			+ mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
		return ans;
	}

	// Cramer method
	std::vector<float> findSolution(float a[3][3], Vec b) {
		float coeff[3][4] = {
			{a[0][0], a[0][1], a[0][2], b.x},
			{a[1][0], a[1][1], a[1][2], b.y},
			{a[2][0], a[2][1], a[2][2], b.z}
		};
		float m1[3][3] = {
			{ coeff[0][3], coeff[0][1], coeff[0][2] },
			{ coeff[1][3], coeff[1][1], coeff[1][2] },
			{ coeff[2][3], coeff[2][1], coeff[2][2] }
		};
		float m2[3][3] = {
			{ coeff[0][0], coeff[0][3], coeff[0][2] },
			{ coeff[1][0], coeff[1][3], coeff[1][2] },
			{ coeff[2][0], coeff[2][3], coeff[2][2] }
		};
		float m3[3][3] = {
			{ coeff[0][0], coeff[0][1], coeff[0][3] },
			{ coeff[1][0], coeff[1][1], coeff[1][3] },
			{ coeff[2][0], coeff[2][1], coeff[2][3] }
		};

		float D = determinantOfMatrix(a);
		float D1 = determinantOfMatrix(m1);
		float D2 = determinantOfMatrix(m2);
		float D3 = determinantOfMatrix(m3);
	
		return std::vector<float> {D1/D, D2/D, -D3/D};
		
	}
};

/**
 * @brief A CSGUnion object derived from Shape.
 *
 * @param transformation	The transformation to the shape.
 * @see Shape.
 */
struct CSGUnion : public Shape {
	std::shared_ptr<Shape> a, b;
	template <class A, class B> CSGUnion(const A &a, const B &b): Shape(), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGUnion(const A &a, const B &b, Transformation transformation): Shape(transformation), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}

	// The first intersection is the one with lower t between the first intersections of a and b
	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};

		HitRecord hitA{a->rayIntersection(invRay)};
		HitRecord hitB{b->rayIntersection(invRay)};
		HitRecord hit;

		if (!hitA.hit and !hitB.hit)
			return HitRecord{};
		else if (!hitA.hit)
			hit = hitB;
		else if (!hitB.hit)
			hit = hitA;
		else if (hitA.t < hitB.t)
			hit = hitA;
		else
			hit = hitB;

		return HitRecord {
			transformation * hit.worldPoint,
			transformation * hit.normal,
			hit.surfacePoint,
			hit.t,
			ray};
	}

	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitA{a->allIntersections(invRay)};
		std::vector<HitRecord> hitB{b->allIntersections(invRay)};
		std::vector<HitRecord> intersections(hitA.size() + hitB.size());
		std::merge(hitA.begin(), hitA.end(), hitB.begin(), hitB.end(), intersections.begin()); // Merge the two ordered vectors into a ordered vector.

		// Apply the CSGUnion transformation to all intersections.
		for (auto &h : intersections) {
			h.worldPoint = transformation * h.worldPoint;
			h.normal = transformation * h.normal;
			h.ray = ray;
		}
		return intersections;
	}

	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return a->isInner(p) or b->isInner(p);
	}
};

/**
 * @brief A CSGDifference object derived from Shape.
 *
 * @param transformation	The transformation to the shape.
 * @see Shape.
 */
struct CSGDifference : public Shape {
	std::shared_ptr<Shape> a, b;
	template <class A, class B> CSGDifference(const A &a, const B &b): Shape(), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGDifference(const A &a, const B &b, Transformation transformation): Shape(transformation), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}

	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		HitRecord hitA{}, hitB{};

		// An intersection with a is also an intersection with a-b iff it is not inside b
		for (auto h : hitListA) {
			if (!b->isInner(h.worldPoint)) {
				hitA = h;
				break;
			}
		}

		// An intersection with b is also an intersection with a-b iff it is inside a
		for (auto h : hitListB) {
			if (a->isInner(h.worldPoint)) {
				hitB = h;
				break;
			}
		}

		// Return the first intersection
		HitRecord hit{};
		if (!hitA.hit and !hitB.hit)
			return HitRecord{};
		else if (!hitA.hit)
			hit = hitB;
		else if (!hitB.hit)
			hit = hitA;
		else if (hitA.t < hitB.t)
			hit = hitA;
		else
			hit = hitB;
		return HitRecord {
			transformation * hit.worldPoint,
			transformation * hit.normal,
			hit.surfacePoint,
			hit.t,
			ray};
	}

	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		std::vector<HitRecord> validA;
		std::vector<HitRecord> validB;


		// An intersection with a is also an intersection with a-b iff it is not inside b
		for (auto h : hitListA) {
			if (!b->isInner(h.worldPoint))
				validA.push_back(h);
		}

		// An intersection with b is also an intersection with a-b iff it is inside a
		for (auto h : hitListB) {
			if (a->isInner(h.worldPoint))
				validB.push_back(h);
		}

		std::vector<HitRecord> intersections(validA.size() + validB.size());
		std::merge(validA.begin(), validA.end(), validB.begin(), validB.end(), intersections.begin()); // Merge two ordered vectors into an ordered vector.
		for (auto &h : intersections) {
			h.worldPoint = transformation * h.worldPoint;
			h.normal = transformation * h.normal;
			h.ray = ray;
		}
		return intersections;
	}

	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return a->isInner(p) and !b->isInner(p);
	}
};

/**
 * @brief A CSGIntersection object derived from Shape.
 *
 * @param transformation	The transformation to the shape.
 * @see Shape.
 */
struct CSGIntersection : public Shape {
	std::shared_ptr<Shape> a, b;
	template <class A, class B> CSGIntersection(const A &a, const B &b): Shape(), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGIntersection(const A &a, const B &b, Transformation transformation): Shape(transformation), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}

	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		HitRecord hitA{}, hitB{};

		// An intersection with a is also an intersection with a-b iff it is not inside b
		for (auto h : hitListA) {
			if (b->isInner(h.worldPoint)) {
				hitA = h;
				break;
			}
		}

		// An intersection with b is also an intersection with a-b iff it is inside a
		for (auto h : hitListB) {
			if (a->isInner(h.worldPoint)) {
				hitB = h;
				break;
			}
		}

		// Return the first intersection
		HitRecord hit{};
		if (!hitA.hit and !hitB.hit)
			return HitRecord{};
		else if (!hitA.hit)
			hit = hitB;
		else if (!hitB.hit)
			hit = hitA;
		else if (hitA.t < hitB.t)
			hit = hitA;
		else
			hit = hitB;
		return HitRecord {
			transformation * hit.worldPoint,
			transformation * hit.normal,
			hit.surfacePoint,
			hit.t,
			ray};
	}

	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		std::vector<HitRecord> validA;
		std::vector<HitRecord> validB;


		// An intersection with a is also an intersection with a-b iff it is not inside b
		for (auto h : hitListA) {
			if (b->isInner(h.worldPoint))
				validA.push_back(h);
		}

		// An intersection with b is also an intersection with a-b iff it is inside a
		for (auto h : hitListB) {
			if (a->isInner(h.worldPoint))
				validB.push_back(h);
		}

		std::vector<HitRecord> intersections(validA.size() + validB.size());
		std::merge(validA.begin(), validA.end(), validB.begin(), validB.end(), intersections.begin()); // Merge two ordered vectors into an ordered vector.
		for (auto &h : intersections) {
			h.worldPoint = transformation * h.worldPoint;
			h.normal = transformation * h.normal;
			h.ray = ray;
		}
		return intersections;
	}

	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return a->isInner(p) and b->isInner(p);
	}
};

/** World class
 * @brief This is the class containing all the shapes of the scene.
 * 
 * @param shapes	List of shapes.
 */
struct World {
	std::vector<std::shared_ptr<Shape>> shapes;
	// make it a template for any shape
	template <class T> void add(const T &newShape){
		shapes.push_back(std::make_shared<T>(newShape));
	}

	HitRecord rayIntersection(Ray ray){
		HitRecord closest{};
		for(int i{}; i < std::size(shapes); i++){
			HitRecord intersection = shapes[i]->rayIntersection(ray);
			if(!intersection.hit)
				continue;
			if((!closest.hit) || (intersection.t < closest.t))
				closest = intersection;
		}
		return closest;
	}
};

#endif // #SHAPE_H
