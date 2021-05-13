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
	return std::fabs(a-b) < epsilon;
}

/**
 * @brief Check whether two 3D objects are close within a distance epsilon
 * 
 * @tparam T The type of the objects to compare. Must have x, y and z fields.
 * @param a	The first object to compare.
 * @param b The second object to be compared.
 * @param epsilon The precision to perform the comparison at
 * @return True if a and b are close, false otherwise
 */
template <typename T> bool areClose(const T &a, const T &b, float epsilon) {
	return _areClose(a.x, b.x, epsilon) and
		_areClose(a.y, b.y, epsilon) and
		_areClose(a.z, b.z, epsilon);
}

bool areMatricesClose(const float a[4][4], const float b[4][4], float epsilon) {
	for (int i{}; i < 4; i++)
		for (int j{}; j < 4; j++)
			if (!_areClose(a[i][j], b[i][j], epsilon))
				return false;
	return true;
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

	Vec(float x = 0.f, float y = 0.f, float z = 0.f): x{x}, y{y}, z{z} {}

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

	Vec operator=(const Vec &other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
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
		return std::sqrt(squaredNorm());
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

	Point(float x = 0.f, float y = 0.f, float z = 0.f) : x{x}, y{y}, z{z} {}
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

	Point operator=(const Point &other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
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

	Vec toVec() {
		return Vec{x, y, z};
	}
};

Point operator+(const Point &a, const Vec &b) {
	return _sum<Point, Vec, Point>(a, b);
}

Point operator-(const Point &a, const Vec &b) {
	return _sum<Point, Vec, Point>(a, -b);
}

struct Normal {
	float x, y, z;
	Normal(float x = 0.f, float y = 0.f, float z = 0.f) : x{x}, y{y}, z{z} {}

	// Compare Normals with a default precision.
	// For a different precision, call areClose<Normal> directly.
	bool operator==(const Normal &other) {
		const float epsilon = 1e-5f;
		return areClose<Normal>(*this, other, epsilon);
	}

	bool operator!=(const Normal &other) {
		return !(*this == other);
	}

	Normal operator-() {
		return Normal{-x, -y, -z};
	}

	Normal operator=(const Normal &other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
};

struct Transformation {
	// The transformation matrix, defaulting to the identity
	float m[4][4] = {{1.f, 0.f, 0.f, 0.f},
			{0.f, 1.f, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f},
			{0.f, 0.f, 0.f, 1.f}};
	// The inverse transformation matrix, defaulting to the identity
	float mInv[4][4] = {{1.f, 0.f, 0.f, 0.f},
			{0.f, 1.f, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f},
			{0.f, 0.f, 0.f, 1.f}};

	// Create a generic transformation overwriting m and mInv
	Transformation(float m[4][4], float mInv[4][4]) {
		for (int i{}; i < 4; i++) {
			for (int j{}; j < 4; j++) {
				this->m[i][j] = m[i][j];
				this->mInv[i][j] = mInv[i][j];
			}
		}
	}

	// Create a diagonal transformation overwriting only the diagonal
	Transformation(float diag[4], float diagInv[4]) {
		for (int i{}; i < 4; i++) {
			m[i][i] = diag[i];
			mInv[i][i] = diagInv[i];
		}
	}

	// Create the identity transformation
	Transformation() {}

	// Transformation composition
	Transformation operator*(Transformation t) {
		float m[4][4] = {0.f}, mInv[4][4] = {0.f};
		for (int i{}; i < 4; i++) {
			for (int j{}; j < 4; j++) {
				for (int k{}; k < 4; k++) {
					m[i][j] += this->m[i][k] * t.m[k][j];
					mInv[i][j] += t.mInv[i][k] * this->mInv[k][j];
				}
			}
		}
		return Transformation{m, mInv};
	}

	// Check whether mInv is the inverse of m
	bool isConsistent(const float epsilon=1e-5) {
		Transformation I;
		Transformation p = (*this)*this->inverse();
		return p.areTransformationsClose(I, epsilon);
	}


	// Transformation of points
	Point operator*(Point p) {
		float x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
		float y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
		float z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
		float w = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];

		if (w != 1) {
			x /= w;
			y /= w;
			z /= w;
		}

		return Point{x, y, z};
	}

	// Transformation of vectors
	Vec operator*(Vec p) {
		float x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z;
		float y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z;
		float z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z;

		return Vec{x, y, z};
	}

	// Transformation of normals (using mInv transposed)
	Normal operator*(Normal n) {
		float x = mInv[0][0] * n.x + mInv[1][0] * n.y + mInv[2][0] * n.z;
		float y = mInv[0][1] * n.x + mInv[1][1] * n.y + mInv[2][1] * n.z;
		float z = mInv[0][2] * n.x + mInv[1][2] * n.y + mInv[2][2] * n.z;

		return Normal{x, y, z};
	}

	// Inverse transformation
	Transformation inverse() {
		float m[4][4];
		float mInv[4][4];

		for (int i{}; i < 4; i++) {
			for (int j{}; j < 4; j++) {
				m[i][j] = this->mInv[i][j];
				mInv[i][j] = this->m[i][j];
			}
		}
		return Transformation{m, mInv};
	}

	// Compare Transformations with a default precision.
	// For a different precision, call areMatricesClose directly.
	bool areTransformationsClose(const Transformation &other, const float epsilon){
		return areMatricesClose(this->m, other.m, epsilon) and
			areMatricesClose(this->mInv, other.mInv, epsilon);
	}
	bool operator==(const Transformation &other) {
		const float epsilon = 1e-5;
		return areTransformationsClose(other, epsilon);
	}

	bool operator!=(const Transformation &other) {
		return !(*this == other);
	}
};

// Function that construct a translation Transformation given a Vec
Transformation translation(const Vec v) {
	float m[4][4]={	{1.f, 0.f, 0.f, v.x}, 
					{0.f, 1.f, 0.f, v.y}, 
					{0.f, 0.f, 1.f, v.z}, 
					{0.f, 0.f, 0.f, 1.f}};
	float mInv[4][4]={	{1.f, 0.f, 0.f, -v.x},
						{0.f, 1.f, 0.f, -v.y},
						{0.f, 0.f, 1.f, -v.z},
						{0.f, 0.f, 0.f, 1.f}};
	return Transformation{m, mInv};
}

// Function that construct a scaling Transformation given a vector of scaling factors
Transformation scaling(const float cx, const float cy, const float cz) {
	float diag[4] = {cx, cy, cz, 1.f};
	float diagInv[4] = {1/cx, 1/cy, 1/cz, 1.f};
	return Transformation(diag, diagInv);
}

/**
 * Construct a rotation around the x axis
 * @param theta The rotation angle, in radians
 * @return The rotation Transformation
 */
Transformation rotationX(const float theta) {
	const float cos = std::cos(theta), sin = std::sin(theta);
	float m[4][4] = {{1.f, 0.f, 0.f, 0.f},
					{0.f, cos, -sin, 0.f}, 
					{0.f, sin, cos, 0.f},
					{0.f, 0.f, 0.f, 1.f}};
	float mInv[4][4] =	{{1.f, 0.f, 0.f, 0.f},
						{0.f, cos, sin, 0.f}, 
						{0.f, -sin, cos, 0.f},
						{0.f, 0.f, 0.f, 1.f}};
	return Transformation{m, mInv};
}

/**
 * Construct a rotation around the y axis
 * @param theta The rotation angle, in radians
 * @return The rotation Transformation
 */
Transformation rotationY(const float theta) {
	const float cos = std::cos(theta), sin = std::sin(theta);
	float m[4][4] = {{cos, 0.f, sin, 0.f},
					{0.f, 1.f, 0.f, 0.f},
					{-sin, 0.f, cos, 0.f},
					{0.f, 0.f, 0.f, 1.f}};
	float mInv[4][4] = 	{{cos, 0.f, -sin, 0.f},
						{0.f, 1.f, 0.f, 0.f}, 
						{sin, 0.f, cos, 0.f},
						{0.f, 0.f, 0.f, 1.f}};
	return Transformation{m, mInv};
}

/**
 * Construct a rotation around the z axis
 * @param theta The rotation angle, in radians
 * @return The rotation Transformation
 */
Transformation rotationZ(const float theta) {
	const float cos = std::cos(theta), sin = std::sin(theta);
	float m[4][4] = {{cos, -sin, 0.f, 0.f}, 
					{sin, cos, 0.f, 0.f},
					{0.f, 0.f, 1.f, 0.f},
					{0.f, 0.f, 0.f, 1.f}};
	float mInv[4][4] = {{cos, sin, 0.f, 0.f}, 
						{-sin, cos, 0.f, 0.f},
						{0.f, 0.f, 1.f, 0.f},
						{0.f, 0.f, 0.f, 1.f}};
	return Transformation{m, mInv};
}

#endif //GEOMETRY_H
