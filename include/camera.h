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

#ifndef CAMERA_H
#define CAMERA_H

#include <geometry.h>
#include <limits>
#include "geometry.h"

struct Ray {
	Point origin;
	Vec dir;
	float tmin, tmax;
	int depth;

	Ray(Point origin = Point(), Vec dir = Vec(), int depth = 0, float tmin = 1e-5, float tmax = std::numeric_limits<float>::infinity()) :
		origin{origin}, dir{dir}, depth{depth}, tmin{tmin}, tmax{tmax} {}

	bool isClose(Ray other, float epsilon) {
		return areClose<Point>(origin, other.origin, epsilon) and
			areClose<Vec>(dir, other.dir, epsilon);
	}

	bool operator==(Ray other) {
		return origin == other.origin and dir == other.dir;
	}

	bool operator!=(Ray other) {
		return !(*this == other);
	}

	/**
	 * Return the Point corresponding to the Ray at the affine parameter t
	 *
	 * @param t The value of the affine parameter to evaluate the Ray at
	 * @return The Point at t
	 */
	Point operator()(float t) {
		return origin + dir * t;
	}
};

Ray operator*(Transformation tr, Ray ray) {
	return Ray(tr * ray.origin, tr * ray.dir, ray.depth, ray.tmin, ray.tmax);
}

/** Camera class
 * 
 * This is the class regarding the observer looking at the scene.
 * The parameters needed are only two:
 * @param	d	The distance between observer and screen.
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @see OrthogonalCamera
 * @see	PerspectiveCamera
 */
struct Camera {
	float d, a;

	Camera(float a) : d{-1.f}, a{a} {}
	
	virtual Ray fireRay(float u, float v);
		/* To be continued...
		float a[3] = {-d, 0.f, 0.f};
		float b[3] = {d, (1.f-2*u)*a, 2*v-1.f};
		*/
};

/** OrthogonalCamera class
 * 
 * This is the class regarding the observer looking at the scene 
 * as if he would be very far from the screen (orthogonal projection).
 * 
 * The parameters needed are only two:
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @param	transformation	The transformation to be applied to the camera (default identity).
 * 
 * This class have a parent:
 * @see	Camera
 */
struct OrthogonalCamera : Camera{
	float a = 1.f;
	Transformation transformation{};

	virtual Ray fireRay(float u, float v) {
		Point origin{-1.f, (1.f-2*u)*a, 2*v-1.f}; //bottom-left is (0, 1, -1)
		Vec dir{1.f, 0.f, 0.f}; // origin_x is -1 so dir cancel that
		return transformation*Ray(origin, dir, 0, 1.f);
	};
};

/** PerspectiveCamera class
 * 
 * This is the class regarding the observer looking at the scene 
 * at a given distance `d`.
 * 
 * The parameters needed are only two:
 * @param	d	The distance between observer and screen (default 1.0).
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @param	transformation	The transformation to be applied to the camera (default identity).
 * 
 * This class have a parent:
 * @see	Camera
 */
struct PerspectiveCamera : Camera{
	float d = 1.f, a = 1.f;
	Transformation transformation{};

	virtual Ray fireRay(float u, float v) {
		Point origin{-d, 0.f, 0.f}; // origin_x is -d, dir cancel that
		Vec dir{d, (1.f - 2*u)*a, 2*v-1.f};
		return transformation*Ray(origin, dir, 0, 1.f);
	};
};

#endif // CAMERA_H
