#include <cmath>
#include "color.h"

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
