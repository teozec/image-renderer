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

#ifndef COLOR_H
#define COLOR_H

#include <algorithm>
#include <string>
#include <sstream>
#include <limits>
#undef NDEBUG
#include <cassert>

struct Color {
	float r, g, b;

	Color operator+(const Color other) {
		return Color{r+other.r, g+other.g, b+other.b};
	}

	Color operator+=(const Color other) {
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}

	Color operator*(const float c) {
		return Color{c*r, c*g, c*b};
	}

	Color operator*=(const float c) {
		r *= c;
		g *= c;
		b *= c;
		return *this;
	}

	Color operator/(const float c) {
		return Color{r/c, g/c, b/c};
	}

	Color operator/=(const float c) {
		r /= c;
		g /= c;
		b /= c;
		return *this;
	}

	Color operator*(const Color other) {
		return Color{other.r*r, other.g*g, other.b*b};
	}

	Color operator*=(const Color other) {
		r *= other.r;
		g *= other.g;
		b *= other.b;
		return *this;
	}

	float &operator[](const size_t i) {
		switch (i) {
		case 0:
			return r;
		case 1:
			return g;
		case 2:
			return b;
		}
		assert(i < 3);
		exit(1);
	}


	operator std::string() const {
		std::ostringstream ss;
		ss << "Color(r=" << r << ", g=" << g << ", b=" << b << ")";
		return ss.str();
	}

	float luminosity() {
		return (std::max({r, g, b}) + std::min({r, g, b})) / 2;
	}

	bool operator==(Color other) {
		const float epsilon = 1e-10f;
		return isClose(other, epsilon);
	}

	bool operator!=(const Color other) {
		return !(*this == other);
	}


	bool isClose(Color other, float epsilon) {
		return std::abs(this->r - other.r) < epsilon and
			std::abs(this->g - other.g) < epsilon and
			std::abs(this->b - other.b) < epsilon;
	}

};

const Color WHITE{1.f, 1.f, 1.f};
const Color BLACK{0.f, 0.f, 0.f};

#endif // COLOR_H
