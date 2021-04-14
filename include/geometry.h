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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

template <typename In1, typename In2, typename Out> Out _sum(const In1 &a, const In2 &b) {
	return Out{a.x + b.x, a.y + b.y, a.z + b.z};
}

struct Vec{
    float x, y, z;

    Vec(float x=0, float y=0, float z=0): x{x}, y{y}, z{z} {}
    Vec(const Vec &) = default;
    Vec(Vec &&) = default;
};

struct Point {
	float x, y, z;

	Point(float x = 0, float y = 0, float z = 0) : x{x}, y{y}, z{z} {}
	Point(const Point &) = default;
	Point(Point &&) = default;

	Point operator-() {
		return Point{-x, -y, -z};
	}
	Vec operator-(const Point &other) {
		_sum<Point, Point, Vec>(*this, -other);
	}

};

Point operator+(const Point &a, const Vec &b) {
	return _sum<Point, Vec, Point>(a, b);
}

#endif //GEOMETRY_H
