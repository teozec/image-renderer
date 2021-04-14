#include <cmath>
#include "geometry.h"

static bool areClose(const float a, const float b, const float epsilon) {
	return fabs(a-b) < epsilon;
}

template <typename T> bool areClose(const T &a, const T &b, float epsilon) {
	return areClose(a.x, b.x, epsilon) and
		areClose(a.y, b.y, epsilon) and
		areClose(a.z, b.z, epsilon);
}
