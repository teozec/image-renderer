#include "hdr-image.h"
#undef NDEBUG
#include <cassert>

int main() {
	HdrImage img(7, 4);

	// Test constructor
	assert(img.width == 7);
	assert(img.height == 4);

	// Test validCoordinates
	assert(img.validCoordinates(0, 0));
	assert(img.validCoordinates(6, 3));
	assert(!img.validCoordinates(7, 3));
	assert(!img.validCoordinates(6, 4));
	assert(!img.validCoordinates(-1, 3));
	assert(!img.validCoordinates(6, -1));

	return 0;
}
