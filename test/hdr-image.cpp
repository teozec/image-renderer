#include "hdr-image.h"
#include "color.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>

using namespace std;

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

	// Test savePfm
	HdrImage img2{3, 2};

	img2.setPixel(0, 0, Color{1.0e1, 2.0e1, 3.0e1});
	img2.setPixel(1, 0, Color{4.0e1, 5.0e1, 6.0e1});
	img2.setPixel(2, 0, Color{7.0e1, 8.0e1, 9.0e1});
	img2.setPixel(0, 1, Color{1.0e2, 2.0e2, 3.0e2});
	img2.setPixel(1, 1, Color{4.0e2, 5.0e2, 6.0e2});
	img2.setPixel(2, 1, Color{7.0e2, 8.0e2, 9.0e2});

	unsigned char referencePfm[] = {
		0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
		0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43,
		0x00, 0x00, 0xc8, 0x43, 0x00, 0x00, 0xfa, 0x43, 0x00, 0x00, 0x16, 0x44,
		0x00, 0x00, 0x2f, 0x44, 0x00, 0x00, 0x48, 0x44, 0x00, 0x00, 0x61, 0x44,
		0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xf0, 0x41,
		0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x70, 0x42,
		0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43
	};
	unsigned int referencePfmLen = 84;
	stringstream buf;
	img2.savePfm(buf);
	char pfmVector[referencePfmLen];
	buf.read(pfmVector, referencePfmLen);
	assert(!memcmp(pfmVector, referencePfm, referencePfmLen));
	
	return 0;
}
