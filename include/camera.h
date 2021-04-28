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

#endif // CAMERA_H
