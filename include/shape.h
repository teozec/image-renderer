/**
 *  Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini
 * 
 * This file is part of image-renderer.
 * 
 * image-renderer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * image-renderer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with image-renderer.  If not, see <https://www.gnu.org/licenses/>. 
 */

#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include "geometry.h"
#include "camera.h"
#include "material.h"

struct Shape;

/** 
 * @brief A record of all the informations related to a ray hitting the shape surface.
 * 
 * @param hit			It's true if the ray actually hits the shape, otherwise is set to false.
 * @param worldPoint	Point of the scene in which the ray intersects the shape.
 * @param normal		Normal to the shape where the ray intersects the shape.
 * @param surfacePoint	Point of the surface where the ray intersects the shape.
 * @param t				Distance from the origin of the ray to the intersection point.
 * @param ray			Ray that hits the shape.
 * @param shape			The @a shared_ptr<Shape> corresponding to the shape hit by the ray.
 * 
 * @see Ray
 * @see Shape
 */
struct HitRecord {
	bool hit = false;
	Point worldPoint;
	Normal normal;
	Vec2D surfacePoint;
	float t;
	Ray ray;
	Material material;
	bool inward;

	HitRecord() {}
	HitRecord(const HitRecord &other) :	//
		hit{other.hit}, worldPoint{other.worldPoint}, normal{other.normal}, //
		surfacePoint{other.surfacePoint}, t{other.t}, ray{other.ray}, // 
		material{other.material}, inward{other.inward} {}
	HitRecord(Point worldPoint, Normal normal, Vec2D surfacePoint, float t, Ray ray, Material material, bool inward) : //
		hit{true}, worldPoint{worldPoint}, normal{normal}, surfacePoint{surfacePoint}, //
		t{t}, ray{ray}, material{material}, inward{inward} {}

	HitRecord operator=(const HitRecord &other) {
		hit = other.hit;
		if (hit) {
			worldPoint = other.worldPoint;
			normal = other.normal;
			surfacePoint = other.surfacePoint;
			t = other.t;
			ray = other.ray;
			material = other.material;
			inward = other.inward;
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
 * @param material			The material of the shape.
 * 
 * @see Sphere
 * @see Plane
 * @see Triangle
 * @see CSGUnion
 * @see CSGDifference
 * @see CSGIntersection
 * @see Box
 */
struct Shape {
	Transformation transformation;
	Material material;

	Shape(): Shape{Transformation{}, Material{}} {}
	Shape(Material material): Shape{Transformation{}, material} {}
	Shape(Transformation transformation): Shape{transformation, Material{}} {}
	Shape(Transformation transformation, Material material): transformation{transformation}, material{material} {}

	/**
	 * @brief Return a HitRecord corresponding to the first intersection between the shape and the ray.
	 */
	virtual HitRecord rayIntersection(Ray ray) = 0;

	/**
	 * @brief Return a vector of HitRecord corresponding to all the intersections, ordered by increasing t.
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) = 0;

	virtual bool isInner(Point p) = 0;

	virtual operator std::string() = 0;
};

/**
 * @brief A unit Sphere object derived from Shape.
 * 
 * @param transformation	The transformation to be applied to the unit sphere centered at the origin.
 * @param material			The material of the sphere.
 * 
 * @see Shape
 */
struct Sphere : public Shape {
	Sphere(): Shape() {}
	Sphere(Transformation transformation): Shape(transformation) {}
	Sphere(Material material): Shape(material) {}
	Sphere(Transformation transformation, Material material): Shape(transformation, material) {}

	/**
	 * @brief 	Return a HitRecord corresponding to the first intersection between the sphere and the ray.
	 * @details It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		float delta4 = (origin.dot(dir)) * (origin.dot(dir)) -
			dir.squaredNorm() * (origin.squaredNorm() - 1.f);

		if (delta4 <= 0.f)
			return HitRecord{};

		float firstHit;
		float sqrtDelta4 = std::sqrt(delta4);
		float t1 = (-origin.dot(dir) - sqrtDelta4) / dir.squaredNorm();
		float t2 = (-origin.dot(dir) + sqrtDelta4) / dir.squaredNorm();
		if (invRay.tmin < t1 and t1 < invRay.tmax)
			return intersection(t1, ray, invRay);
		else if (invRay.tmin < t2 and t2 < invRay.tmax)
			return intersection(t2, ray, invRay);
		else
			return HitRecord{};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t. 
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		std::vector<HitRecord> intersections;

		float delta4 = (origin.dot(dir)) * (origin.dot(dir)) -
			dir.squaredNorm() * (origin.squaredNorm() - 1.f);

		if (delta4 <= 0.f)
			return intersections;

		float sqrtDelta4 = std::sqrt(delta4);
		float t1 = (-origin.dot(dir) - sqrtDelta4) / dir.squaredNorm();
		float t2 = (-origin.dot(dir) + sqrtDelta4) / dir.squaredNorm();
		if (invRay.tmin < t1 and t1 < invRay.tmax)
			intersections.push_back(intersection(t1, ray, invRay));
		if (invRay.tmin < t2 and t2 < invRay.tmax)
			intersections.push_back(intersection(t2, ray, invRay));

		return intersections;
	}

	/**
	 * @brief Check if a point is inside the sphere.
	 * 
	 * @param p 
	 * @return true 
	 * @return false 
	 */
	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return p.x * p.x + p.y * p.y + p.z * p.z < 1.f;
	}

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "Sphere";
		return ss.str();
	}

private:
	/**
	 * @brief Wrapper of intersection info.
	 * 
	 * @param t 
	 * @param ray 
	 * @param invRay 
	 * @return HitRecord 
	 */
	HitRecord intersection(float t, Ray ray, Ray invRay) {
		Point hitPoint{invRay(t)};
		bool inward = !isInner(hitPoint - invRay.dir*1e-4f);
		return HitRecord{
			transformation * hitPoint,
			transformation * sphereNormal(hitPoint, invRay.dir),
			spherePointToUV(hitPoint),
			t,
			ray,
			material,
			inward};
	}

	/**
	 * @brief Return the normal at a given point coming from a specific direction.
	 * 
	 * @param p 		Point.
	 * @param dir 		Direction of the incoming ray.
	 * @return Normal 
	 */
	Normal sphereNormal(Point p, Vec dir) {
		Normal result{p.x, p.y, p.z};
		return p.toVec().dot(dir) < 0. ? result : -result;
	}

	/**
	 * @brief 2D sphere surface coordinate system converter (from 3D world point).
	 * 
	 * @param p		3D world point.
	 * @return Vec2D 
	 */
	Vec2D spherePointToUV(Point p) {
		float v = std::atan2(p.y, p.x) / (float) (2 * M_PI);
		return Vec2D{std::acos(p.z) / (float) M_PI, v>=0 ? v : v+1};
	}
};

/**
 * @brief An ininite xy-plane object derived from Shape.
 * 
 * @param transformation	The transformation to be applied to the infinite xy-plane passing through the origin.
 * @param material			The material of the plane.
 * 
 * @see Shape
 */
struct Plane : public Shape {
	int scale;
	Plane(): Shape() {}
	Plane(Transformation transformation): Shape(transformation) {}
	Plane(Material material, int scale=1): Shape(material), scale{scale} {}
	Plane(Transformation transformation, Material material, int scale=1): Shape(transformation, material), scale{scale} {}

	/**
	 * @brief 	Return a HitRecord corresponding to the intersection between the ray and the plane.
	 * @details It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		const float epsilon = 1e-5;

		if (std::abs(dir.z - 0.f) < epsilon)
			return HitRecord{};

		float t = -origin.z / dir.z;
		if (invRay.tmin > t or t > invRay.tmax)
			return HitRecord{};

		Point hitPoint{invRay(t)};
		bool inward = !isInner(hitPoint - invRay.dir*1e-4f);
		return HitRecord{
			transformation * hitPoint,
			transformation * planeNormal(hitPoint, invRay.dir),
			planePointToUV(hitPoint),
			t,
			ray,
			material,
			inward};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t. 
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		HitRecord hit = rayIntersection(ray);
		if (hit.hit)
			return std::vector<HitRecord>{hit};
		else
			return std::vector<HitRecord>{};
	}

	/**
	 * @brief By convention, the plane inner part is the z<0 half space
	 * 
	 * @param p 
	 * @return true 
	 * @return false 
	 */
	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return p.z < 0;
	}

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "Plane";
		return ss.str();
	}

private:

	/**
	 * @brief Return the normal of the plane at a given point, with a ray coming at a given direction.
	 * 
	 * @param p 
	 * @param dir 
	 * @return Normal 
	 */
	Normal planeNormal(Point p, Vec dir) {
		Normal result{0.f, 0.f, 1.f};
		return dir.z < 0. ? result : -result;
	}

	/**
	 * @brief 2D plane surface coordinate system converter (from 3D world point).
	 * 
	 * @param p 
	 * @return Vec2D 
	 */
	Vec2D planePointToUV(Point p) {
		//Move the origin of uv coords and scale up (or down) the texture map
		Vec origin{-10.f, 10.f, -10.f};
		Point ref = (p-origin)*(1.f/scale);
		return Vec2D{(ref.x - std::floor(ref.x)), (ref.y - std::floor(ref.y))};
	}
};

/**
 * @brief A triangle object derived from Shape.
 * 
 * @param A					A vertex.
 * @param B					A vertex.
 * @param C					A vertex.
 * @param transformation	The transformation to be applied to the triangle.
 * @param material			The material of the triangle.
 * 
 * @see Shape
 */
struct Triangle : public Shape {

	Triangle(): Shape() {}
	Triangle(Transformation transform): Shape(transform) {
		A = transform*(this->A);
		B = transform*(this->B);
		C = transform*(this->C);
	}
	Triangle(Transformation transform, Material material): Shape(transform, material) {
		A = transform*(this->A);
		B = transform*(this->B);
		C = transform*(this->C);
	} 
	Triangle(Point a, Point b, Point c, Transformation transform = Transformation{}) : Shape(transform) {
		A = transform*a;
		B = transform*b;
		C = transform*c;
	}
	Triangle(Point a, Point b, Point c, Transformation transform, Material material) : Shape(transform, material) {
		A = transform*a;
		B = transform*b;
		C = transform*c;
	}

	/**
	 * @brief 	Return a HitRecord corresponding to the intersection between the ray and the triangle.
	 * @details It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
	virtual HitRecord rayIntersection(Ray ray) override {
		float s[3][3] = {{(B-A).x, (C-A).x, ray.dir.x},
						{(B-A).y, (C-A).y, ray.dir.y},
						{(B-A).z, (C-A).z, ray.dir.z}};
		Vec b{ray.origin-A};
		float beta, gamma, t;

		if (determinantOfMatrix(s) == 0.f)
			return HitRecord{};

		std::vector<float> solution = findSolution(s, b);
		if (!(ray.tmin < -solution[2] && -solution[2] < ray.tmax)
			|| !(0 < solution[1] && solution[1] < 1)
			|| !(0 < solution[0] && solution[0] < 1)
			|| !(0 < 1-solution[0]-solution[1] && 1-solution[0]-solution[1] < 1)) {
			return HitRecord{};
		}

		t = -solution[2];
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
			ray,
			material,
			false};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t. 
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		HitRecord hit = rayIntersection(ray);
		if (hit.hit)
			return std::vector<HitRecord>{hit};
		else
			return std::vector<HitRecord>{};
	}

	/**
	 * @deprecated Not implemented
	 */
	virtual bool isInner(Point p) override {
		return false;
	}

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "Triangle";
		return ss.str();
	}

private:

	Point A{0.f, 0.f, 0.f}, B{0.f, 1.f, 0.f}, C{0.f, 0.f, 1.f};

	/**
	 * @brief Return the normal to the triangle at a given point, with a ray coming at a given direction.
	 * 
	 * @param p 
	 * @param dir 
	 * @return Normal 
	 */
  	Normal triangleNormal(Vec p, Vec dir) {
		p.normalize();
		Normal result{p.x, p.y, p.z};
		return dir.dot(p) < 0. ? result : -result;
	}

	/**
	 * @brief 2D plane surface coordinate system converter (from barycentric coordinates).
	 * 
	 * @param beta 
	 * @param gamma 
	 * @return Vec2D 
	 */
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

	/**
	 * @brief Cramer method.
	 * 
	 * 
	 * @param a 
	 * @param b 
	 * @return std::vector<float> 
	 */
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
	
		return std::vector<float> {D1/D, D2/D, D3/D};
		
	}
};

/**
 * @brief A CSGUnion object derived from Shape.
 *
 * @tparam A	First shape type.
 * @tparam B 	Second shape type.
 * @param a		First shape.
 * @param b		Second shape.
 * @param transformation	The transformation to the shape.
 * @see Shape.
 */
struct CSGUnion : public Shape {
	std::shared_ptr<Shape> a, b;
	CSGUnion(std::shared_ptr<Shape> a, std::shared_ptr<Shape> b, Transformation transformation = Transformation{}): a{a}, b{b}, Shape(transformation) {}
	template <class A, class B> CSGUnion(const A &a, const B &b): Shape(), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGUnion(const A &a, const B &b, Material material): Shape(material), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGUnion(const A &a, const B &b, Transformation transformation): Shape(transformation), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGUnion(const A &a, const B &b, Transformation transformation, Material material): Shape(transformation, material), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}

	/**
	 * @brief	Return a HitRecord corresponding to the intersection between the ray and the CSGUnion.
	 * @details The first intersection is the one with lower t between the first intersections of a and b. It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
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

		bool inward = !isInner(hit.worldPoint - invRay.dir*1e-4f);
		return HitRecord {
			transformation * hit.worldPoint,
			transformation * hit.normal,
			hit.surfacePoint,
			hit.t,
			ray,
			hit.material,
			inward};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t. 
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
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

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "CSGUnion";
		return ss.str();
	}
};

/**
 * @brief A CSGDifference object derived from Shape.
 * 
 * @tparam A	First shape type.
 * @tparam B 	Second shape type.
 * @param a		First shape.
 * @param b		Second shape.
 * @param transformation	The transformation to the shape.
 * @param material			The material of the shape.
 * @see Shape.
 */
struct CSGDifference : public Shape {
	std::shared_ptr<Shape> a, b;
	CSGDifference(std::shared_ptr<Shape> a, std::shared_ptr<Shape> b, Transformation transformation = Transformation{}): a{a}, b{b}, Shape(transformation) {}
	template <class A, class B> CSGDifference(const A &a, const B &b): Shape(), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGDifference(const A &a, const B &b, Material material): Shape(material), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGDifference(const A &a, const B &b, Transformation transformation): Shape(transformation), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGDifference(const A &a, const B &b, Transformation transformation, Material material): Shape(transformation, material), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}

	/**
	 * @brief 	Return a HitRecord corresponding to the intersection between the ray and the CSGDifference.
	 * @details It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		HitRecord hitA{}, hitB{};

		// An intersection with 'a' is also an intersection with 'a - b' iff it is not inside 'b'
		for (auto h : hitListA) {
			if (!b->isInner(h.worldPoint)) {
				hitA = h;
				break;
			}
		}

		// An intersection with 'b' is also an intersection with 'a - b' iff it is inside 'a'
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

		bool inward = !isInner(hit.worldPoint - invRay.dir*1e-4f);
		return HitRecord {
			transformation * hit.worldPoint,
			transformation * hit.normal,
			hit.surfacePoint,
			hit.t,
			ray,
			hit.material,
			inward};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t. 
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		std::vector<HitRecord> validA;
		std::vector<HitRecord> validB;


		// An intersection with 'a' is also an intersection with 'a - b' iff it is not inside 'b'
		for (auto h : hitListA) {
			if (!b->isInner(h.worldPoint))
				validA.push_back(h);
		}

		// An intersection with 'b' is also an intersection with 'a - b' iff it is inside 'a'
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

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "CSGDifference";
		return ss.str();
	}
};

/**
 * @brief A CSGIntersection object derived from Shape.
 * 
 * @tparam A	First shape type.
 * @tparam B 	Second shape type.
 * @param a		First shape.
 * @param b		Second shape.
 * @param transformation	The transformation to the shape.
 * @param material			The material of the shape.
 * @see Shape.
 */
struct CSGIntersection : public Shape {
	std::shared_ptr<Shape> a, b;
	CSGIntersection(std::shared_ptr<Shape> a, std::shared_ptr<Shape> b, Transformation transformation = Transformation{}): a{a}, b{b}, Shape(transformation) {}
	template <class A, class B> CSGIntersection(const A &a, const B &b): Shape(), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGIntersection(const A &a, const B &b, Material material): Shape(material), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGIntersection(const A &a, const B &b, Transformation transformation): Shape(transformation), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}
	template <class A, class B> CSGIntersection(const A &a, const B &b, Transformation transformation, Material material): Shape(transformation, material), a{std::make_shared<A>(a)}, b{std::make_shared<B>(b)} {}

	/**
	 * @brief 	Return a HitRecord corresponding to the intersection between the ray and the CSGIntersection.
	 * @details It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		HitRecord hitA{}, hitB{};

		// An intersection with 'a' is also an intersection with 'a - b' iff it is inside 'b'
		for (auto h : hitListA) {
			if (b->isInner(h.worldPoint)) {
				hitA = h;
				break;
			}
		}

		// An intersection with 'b' is also an intersection with 'a - b' iff it is inside 'a'
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
		
		bool inward = !isInner(hit.worldPoint - invRay.dir*1e-4f);
		return HitRecord {
			transformation * hit.worldPoint,
			transformation * hit.normal,
			hit.surfacePoint,
			hit.t,
			ray,
			hit.material,
			inward};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t. 
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay{transformation.inverse() * ray};
		std::vector<HitRecord> hitListA = a->allIntersections(invRay);
		std::vector<HitRecord> hitListB = b->allIntersections(invRay);
		std::vector<HitRecord> validA;
		std::vector<HitRecord> validB;

		// An intersection with 'a' is also an intersection with 'a - b' iff it is inside 'b'
		for (auto h : hitListA) {
			if (b->isInner(h.worldPoint))
				validA.push_back(h);
		}

		// An intersection with 'b' is also an intersection with 'a - b' iff it is inside 'a'
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

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "CSGIntersection";
		return ss.str();
	}
};

/**
 * @brief An axis aligned box object derived from Shape.
 * 
 * @param pMin		First end of the cube diagonal.
 * @param pMax		Second end of the cube diagonal.
 * @param transformation	The transformation to the shape.
 * @param material			The material of the shape.
 * @see Shape.
 */
struct Box : Shape {
	Point pMin, pMax;
	Box(Point pMin, Point pMax, Material material): pMin{pMin}, pMax{pMax}, Shape(material) {
		assert(pMin.x < pMax.x);
		assert(pMin.y < pMax.y);
		assert(pMin.z < pMax.z);
	}
	Box(Point pMin, Point pMax, Transformation transformation = Transformation()): pMin{pMin}, pMax{pMax}, Shape(transformation) {
		assert(pMin.x < pMax.x);
		assert(pMin.y < pMax.y);
		assert(pMin.z < pMax.z);
	}
	Box(Point pMin, Point pMax, Transformation transformation, Material material): pMin{pMin}, pMax{pMax}, Shape(transformation, material) {
		assert(pMin.x < pMax.x);
		assert(pMin.y < pMax.y);
		assert(pMin.z < pMax.z);
	}

	/**
	 * @brief 	Return a HitRecord corresponding to the intersection between the ray and the box.
	 * @details It must be inside the range ['tmin', 'tmax'].
	 * 
	 * @param ray 
	 * @return HitRecord 
	 */
	virtual HitRecord rayIntersection(Ray ray) override {
		Ray invRay = transformation.inverse() * ray;
		if (!intersection(invRay))
			return HitRecord{};

		float t;
		Normal normal;
		int face;
		if (invRay.tmin < tMin and tMin < invRay.tmax) {
			t = tMin;
			face = faceMin;
			normal = boxNormal(faceMin);
		} else if (invRay.tmin < tMax and tMax < invRay.tmax) {
			t = tMax;
			face = faceMax;
			normal = -boxNormal(faceMax);
		} else {
			return HitRecord{};
		}
		Point hitPoint{invRay(t)};
		bool inward = !isInner(hitPoint - invRay.dir*1e-4f);
		return HitRecord{
			transformation * hitPoint,
			transformation * normal,
			boxPointToUV(hitPoint, face),
			t,
			ray,
			material,
			inward
		};
	}

	/**
	 * @brief 	Return a vector of HitRecords of all the intersections.
	 * @details The records are ordered by increasing t.
	 * 
	 * @param ray 
	 * @return std::vector<HitRecord> 
	 */
	virtual std::vector<HitRecord> allIntersections(Ray ray) override {
		Ray invRay = transformation.inverse() * ray;
		std::vector<HitRecord> intersections;

		if (!intersection(invRay))
			return intersections;

		if (invRay.tmin < tMin and tMin < invRay.tmax) {
			Normal normal{boxNormal(faceMin)};
			Point hitPoint{invRay(tMin)};
			bool inward = !isInner(hitPoint - invRay.dir*1e-4f);
			intersections.push_back(HitRecord{
				transformation * hitPoint,
				transformation * normal,
				boxPointToUV(hitPoint, faceMin),
				tMin,
				ray,
				material,
				inward
			});
		}
		if (invRay.tmin < tMax and tMax < invRay.tmax) {
			Normal normal{-boxNormal(faceMax)};
			Point hitPoint{invRay(tMax)};
			bool inward = !isInner(hitPoint - invRay.dir*1e-4f);
			intersections.push_back(HitRecord{
				transformation * hitPoint,
				transformation * normal,
				boxPointToUV(hitPoint, faceMax),
				tMax,
				ray,
				material,
				inward
			});
		}
		return intersections;
	}

	virtual bool isInner(Point p) override {
		p = transformation.inverse() * p;
		return pMin.x < p.x and p.x < pMax.x and
			pMin.y < p.y and p.y < pMax.y and
			pMin.z < p.z and p.z < pMax.z;
	}

	virtual operator std::string() override {
		std::ostringstream ss;
		ss << "Box";
		return ss.str();
	}

private:
	float tMin;
	float tMax;

	/* The faces of te box are as follows:
	 *  0: lower yz face
	 *  1: lower xz face
	 *  2: lower xy face
	 *  3: upper yz face
	 *  4: upper xz face
	 *  5: upper xy face
	 */
	int faceMin;
	int faceMax;

	/**
	 * @brief Returns true if the ray intersects the box, false otherwise.
	 * @details It also sets the private members tMin, tMax, faceMin, faceMax corresponding to the first and second hit t and face, respectively.
	 * 
	 * @param invRay 
	 * @return true 
	 * @return false 
	 */
	bool intersection(Ray invRay) {
		float t1, t2;
		Vec origin{invRay.origin.toVec()}, dir{invRay.dir};
		const float epsilon = 1e-5;
		tMin = FLT_MIN;
		tMax = FLT_MAX;
		for (int i{}; i < 3; i++) {
			// Ray parallel to one of the axes
			if (std::abs(dir[i] - 0.f) < epsilon){
				if (pMin[i] < origin[i] and origin[i] < pMax[i])
					continue;
				else
					return false;
			}
			t1 = (pMin[i] - origin[i]) / dir[i];
			t2 = (pMax[i] - origin[i]) / dir[i];

			if (t1 < t2) {
				// First hit face 0, 1 or 2 (min faces), then 3, 4 or 5 (max faces)
				// Update global minimum
				if (t1 > tMin) {
					tMin = t1;
					faceMin = i;
				}
				// Update global maximum
				if (t2 < tMax) {
					tMax = t2;
					faceMax = i + 3;
				}
			} else {
				// First hit face 3, 4 or 5 (max faces), then 0, 1 or 2 (min faces)
				// Update global minimum
				if (t2 > tMin) {
					tMin = t2;
					faceMin = i + 3;
				}
				// Update global maximum
				if (t1 < tMax) {
					tMax = t1;
					faceMax = i;
				}
			}

			// No overlap between intervals
			if (tMin > tMax)
				return false;
		}
		return true;
	}

	/**
	 * @brief Return the normal to the box at a given face.
	 * 
	 * @param face 
	 * @return Normal 
	 */
	Normal boxNormal(int face) {
		switch (face) {
		case 0:
			return Normal{-1.f, 0.f, 0.f};
		case 1:
			return Normal{0.f, -1.f, 0.f};
		case 2:
			return Normal{0.f, 0.f, -1.f};
		case 3:
			return Normal{1.f, 0.f, 0.f};
		case 4:
			return Normal{0.f, 1.f, 0.f};
		case 5:
			return Normal{0.f, 0.f, 1.f};
		}
		assert(0 <= face and face < 6);
		return Normal{}; // To remove the compiler warning. We won't get here because of the assert.
	}

	/**
	 * @brief 	2D box surface coordinate system converter (from 3D world point).
	 * @details The [0, 1] interval is divided in 6 equal subintervals [i/6, (i+1)/6], where i is the number of the face (0-5).
	 * 
	 * @param hitPoint 	3D point of intersection.
	 * @param face 		Face hitted.
	 * @return Vec2D 
	 */
	Vec2D boxPointToUV(Point hitPoint, int face) {
		float u, v;
		switch (face) {
		// u: y, v: z
		case 0:
		case 3:
			u = (face + (hitPoint.y - pMin.y) / (pMax.y - pMin.y)) / 6.f;
			v = (face + (hitPoint.z - pMin.z) / (pMax.z - pMin.z)) / 6.f;
			break;
		// u: x, v: z
		case 1:
		case 4:
			u = (face + (hitPoint.x - pMin.x) / (pMax.x - pMin.x)) / 6.f;
			v = (face + (hitPoint.z - pMin.z) / (pMax.z - pMin.z)) / 6.f;
			break;
		// u: x, v: y
		case 2:
		case 5:
			u = (face + (hitPoint.x - pMin.x) / (pMax.x - pMin.x)) / 6.f;
			v = (face + (hitPoint.y - pMin.y) / (pMax.y - pMin.y)) / 6.f;
			break;
		}
		return Vec2D{u, v};
	}
};

/**
 * @brief This is the class containing all the shapes of the scene.
 * 
 * @param shapes	List of shapes.
 * 
 * @see Shape
 */
struct World {
	std::vector<std::shared_ptr<Shape>> shapes;
	// make it a template for any shape
	template <class T> void add(const T &newShape){
		shapes.push_back(std::make_shared<T>(newShape));
	}

	HitRecord rayIntersection(Ray ray) {
		HitRecord closest{};
		for(int i{}; i < std::size(shapes); i++) {
			HitRecord intersection = shapes[i]->rayIntersection(ray);
			if(!intersection.hit)
				continue;
			if((!closest.hit) or (intersection.t < closest.t))
				closest = intersection;
		}
		return closest;
	}

	operator std::string() {
		std::ostringstream ss;
		for (auto shape : shapes)
			ss << std::string{*shape} << std::endl;
		return ss.str();
	}

};

// ASSETS
	
/**
 * @brief This is a predefinite chair that lays on the floor (plane xy).
 * 
 * @return CSGUnion 
 */
CSGUnion Chair(Transformation transformation, Material material){
	Box legFrontLeft{Point{-.5f, .3f, -1.5f}, Point{-.3f, .5f, -.5f}, material};
	Box legFrontRight{Point{-.5f, .3f, -1.5f}, Point{-.3f, .5f, -.5f}, scaling(1.f, -1.f, 1.f), material};
	CSGUnion frontLegs{legFrontLeft, legFrontRight};
	Box legBackLeft{Point{-.5f, .3f, -1.5f}, Point{-.3f, .5f, -.5f}, scaling(-1.f, 1.f, 1.f), material};
	Box legBackRight{Point{-.5f, .3f, -1.5f}, Point{-.3f, .5f, -.5f}, scaling(-1.f, -1.f, 1.f), material};
	CSGUnion backLegs{legBackLeft, legBackRight};
	CSGUnion legs{frontLegs, backLegs};
	Box bottom{Point{-.5f, -.5f, -.5f}, Point{.5f, .5f, -.3f}, material};
	CSGIntersection back{Box{Point{.3f, -.5f, -.3f}, Point{.5f, .5f, 1.f}, material}, Sphere{translation(Vec{.4f, 0.f, 0.f}), material}};
	CSGUnion top{bottom, back};
	return CSGUnion{legs, top, transformation*translation(Vec{0.f, 0.f, 1.5f})};
};

/**
 * @brief This is a predefinite dice that lays on the floor (plane xy).
 * 
 * @return CSGIntersection
 */
CSGIntersection Dice(Transformation transformation, Material diceMat, Material numbersMat){
	Box cube{Point{-.5f, -.5f, -.5f}, Point{.5f, .5f, .5f}, diceMat};

	Sphere one{translation(Vec{0.f, 0.f, -.5f})*scaling(.1f), numbersMat};
	CSGDifference face1{cube, one};

	CSGUnion two{
		Sphere{translation(Vec{0.3f, 0.f, 0.3f})*translation(Vec{0.f, .5f, 0.f})*scaling(.1f), numbersMat}, 
		Sphere{translation(Vec{-0.3f, 0.f, -0.3f})*translation(Vec{0.f, .5f, 0.f})*scaling(.1f), numbersMat}, 
		numbersMat};
	CSGDifference face2{face1, two};

	CSGUnion three{
		Sphere{translation(Vec{0.5f, 0.f, 0.f})*scaling(.1f), numbersMat}, 
		CSGUnion{
			Sphere{translation(Vec{0.f, 0.3f, 0.3f})*translation(Vec{.5f, 0.f, 0.f})*scaling(.1f), numbersMat},
			Sphere{translation(Vec{0.f, -0.3f, -0.3f})*translation(Vec{.5f, 0.f, 0.f})*scaling(.1f), numbersMat}
		}
	};
	CSGDifference face3{face2, three};

	CSGUnion four{
		Sphere{translation(Vec{0.f, 0.3f, 0.3f})*translation(Vec{-.5f, 0.f, 0.f})*scaling(.1f), numbersMat}, 
		CSGUnion{
			Sphere{translation(Vec{0.f, -0.3f, 0.3f})*translation(Vec{-.5f, 0.f, 0.f})*scaling(.1f), numbersMat},
			CSGUnion{
				Sphere{translation(Vec{0.f, 0.3f, -0.3f})*translation(Vec{-.5f, 0.f, 0.f})*scaling(.1f), numbersMat},
				Sphere{translation(Vec{0.f, -0.3f, -0.3f})*translation(Vec{-.5f, 0.f, 0.f})*scaling(.1f), numbersMat}
			}
		}
	};
	CSGDifference face4{face3, four};

	CSGUnion five{
		Sphere{translation(Vec{0.f, -.5f, 0.f})*scaling(.1f), numbersMat}, 
		CSGUnion{
			Sphere{translation(Vec{0.3f, 0.f, 0.3f})*translation(Vec{0.f, -.5f, 0.f})*scaling(.1f), numbersMat}, 
			CSGUnion{
				Sphere{translation(Vec{-0.3f, 0.f, 0.3f})*translation(Vec{0.f, -.5f, 0.f})*scaling(.1f), numbersMat},
				CSGUnion{
					Sphere{translation(Vec{0.3f, 0.f, -0.3f})*translation(Vec{0.f, -.5f, 0.f})*scaling(.1f), numbersMat},
					Sphere{translation(Vec{-0.3f, 0.f, -0.3f})*translation(Vec{0.f, -.5f, 0.f})*scaling(.1f), numbersMat}
				}
			}
		}
	};
	CSGDifference face5{face4, five};

	CSGUnion six{
		Sphere{translation(Vec{0.3f, 0.f, 0.f})*translation(Vec{0.f, 0.f, .5f})*scaling(.1f), numbersMat}, 
		CSGUnion{	
			Sphere{translation(Vec{-0.3f, 0.f, 0.f})*translation(Vec{0.f, 0.f, .5f})*scaling(.1f), numbersMat}, 
			CSGUnion{
				Sphere{translation(Vec{0.3f, 0.3f, 0.f})*translation(Vec{0.f, 0.f, .5f})*scaling(.1f), numbersMat}, 
				CSGUnion{
					Sphere{translation(Vec{-0.3f, 0.3f, 0.f})*translation(Vec{0.f, 0.f, .5f})*scaling(.1f), numbersMat},
					CSGUnion{
						Sphere{translation(Vec{0.3f, -0.3f, 0.f})*translation(Vec{0.f, 0.f, .5f})*scaling(.1f), numbersMat},
						Sphere{translation(Vec{-0.3f, -0.3f, 0.f})*translation(Vec{0.f, 0.f, .5f})*scaling(.1f), numbersMat}
					}
				}
			}
		}
	};
	CSGDifference face6{face5, six};

	return CSGIntersection{face6, Sphere{scaling(.8f)}, transformation};
}

#endif // #SHAPE_H
