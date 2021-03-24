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

#include "color.h"
#include <cmath>

static bool areClose(const float a, const float b, const float epsilon) {
	return fabs(a-b) < epsilon;
}

bool areColorsClose(const Color a, const Color b, const float epsilon) {
	return areClose(a.r, b.r, epsilon) and
		areClose(a.g, b.g, epsilon) and
		areClose(a.b, b.b, epsilon);
}

bool Color::operator==(const Color other) {
	const float epsilon = 1e-10f;
	return areColorsClose(*this, other, epsilon);
}
