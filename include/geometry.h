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
#include <string>
#include <sstream>
#include <cmath>

static bool _areClose(const float a, const float b, const float epsilon) {
	return fabs(a-b) < epsilon;
}

template <typename T> bool areClose(const T &a, const T &b, float epsilon) {
	return _areClose(a.x, b.x, epsilon) and
		_areClose(a.y, b.y, epsilon) and
		_areClose(a.z, b.z, epsilon);
}

template <typename In1, typename In2, typename Out> Out _sum(const In1 &a, const In2 &b) {
	return Out{a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T> T scalarMultiplication(const T &a, const float c) {
    return T{a.x*c, a.y*c, a.z*c};
}
struct Vec {
	//Coordinates
	float x, y, z;

	Vec(float x=0, float y=0, float z=0): x{x}, y{y}, z{z} {}
	Vec(const Vec &) = default;
	Vec(Vec &&) = default;

	// Convert Vec to a human readable string with the values of its elements
	operator std::string() const {
		std::ostringstream ss;
		ss << "Vec(x=" << x << ", y=" << y << ", z=" << z;
		return ss.str();
	}

	// Basic vector operations
	bool operator==(const Vec &other) {
		const float epsilon = 1e-10f;
		return areClose<Vec>(*this, other, epsilon);
	}

	bool operator!=(const Vec &other) {
		return !(*this == other);
	}

	Vec operator+(const Vec &other) {
		return _sum<Vec, Vec, Vec>(*this, other);
	}

	Vec operator-() const { 
        return Vec{-x, -y, -z}; 
    }

	Vec operator-(const Vec &other) {
		return _sum<Vec, Vec, Vec>(*this, -other);
	}

	Vec operator*(const float c) {
		return scalarMultiplication<Vec>(*this, c);
	}

	float dot(const Vec &other) const {
		return x*other.x + y*other.y + z*other.z;
	}

	Vec cross(const Vec &other) {
		return Vec{y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x};
	}

	// Basic vector methods
	float squaredNorm() const {
		return dot(*this);
	}

	float norm() const {
		return sqrt(squaredNorm());
	}

	// Normalize Vec in place
	void normalize() {
		float n = norm();
		x /= n;
		y /= n;
		z /= n;
	}

	// Return normalized copy of Vec
	Vec versor() const {
		float n = norm();
		return Vec{ x/n, y/n, z/n };
	}
};

struct Point {
	float x, y, z;

	Point(float x = 0, float y = 0, float z = 0) : x{x}, y{y}, z{z} {}
	Point(const Point &) = default;
	Point(Point &&) = default;

    Point operator*(const float c) {
        return scalarMultiplication<Point>(*this, c);
    }

    Point operator+(const Point &other) {
        return _sum<Point, Point, Point>(*this, other);
    }

	Point operator-() const {
		return Point{-x, -y, -z};
	}

	Vec operator-(const Point &other) {
		return _sum<Point, Point, Vec>(*this, -other);
	}

	// Compare Points with a default precision.
	// For a different precision, call areClose<Point> directly.
	bool operator==(const Point &other) {
		const float epsilon = 1e-10f;
		return areClose<Point>(*this, other, epsilon);
	}

	bool operator!=(const Point &other) {
		return !(*this == other);
	}

	// Convert Point to a human readable string with the values of its elements
	operator std::string() const {
		std::ostringstream ss;
		ss << "Point(x=" << x << ", y=" << y << ", z=" << z;
		return ss.str();
	}
};

Point operator+(const Point &a, const Vec &b) {
	return _sum<Point, Vec, Point>(a, b);
}

Point operator-(const Point &a, const Vec &b) {
	return _sum<Point, Vec, Point>(a, -b);
}

#endif //GEOMETRY_H
