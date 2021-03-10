#include <cmath>
#include "color.h"

#define EPSILON 1e-10f

Color Color::operator+(const Color other) {
	return Color{other.r+r, other.g+g, other.b+b};
}

static bool areClose(float a, float b, float epsilon) {
	return fabs(a-b) < epsilon;
}

bool areColorsClose(const Color a, const Color b, float epsilon) {
	return areClose(a.r, b.r, epsilon) and
		areClose(a.g, b.g, epsilon) and
		areClose(a.b, b.b, epsilon);
}

bool Color::operator==(const Color other) {
	return areColorsClose(*this, other, EPSILON);
}

bool Color::operator!=(const Color other) {
	return !(*this == other);
}
