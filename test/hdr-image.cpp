#include "hdr-image.h"
#include "color.h"
#undef NDEBUG
#include <cassert>

int main() {
	HdrImage img{7, 4};

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

	// Test pixelOffset	
	assert(img.pixelOffset(0, 0) == 0);
	assert(img.pixelOffset(3, 2) == (3*4 + 2));

	// Test getPixel
	Color ref{1.0, 2.0, 3.0};
	img.setPixel(3, 2, ref);
	assert(img.getPixel(3, 2) == ref);
	
	return 0;
}
