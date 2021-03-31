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

struct Color {
	float r, g, b;
    
	Color operator+(const Color other) {
		return Color{r+other.r, g+other.g, b+other.b};
	}

	Color operator*(const float c) {
		return Color{c*r, c*g, c*b};
	}

	Color operator*(const Color other) {
		return Color{other.r*r, other.g*g, other.b*b};
	}

	float luminosity() {
		return (std::max({r, g, b}) + std::min({r, g, b})) / 2;
	}

	bool operator==(Color other);

	bool operator!=(const Color other) {
		return !(*this == other);
	}
};

bool areColorsClose(Color a, Color b, float epsilon);

#endif // COLOR_H
