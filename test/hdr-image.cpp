#include "hdr-image.h"
#undef NDEBUG
#include <cassert>

int main() {
	img = HdrImage{7, 4}
	
	assert(img.pixelOffset(0,0) == 0);
	assert(img.pixelOffset(3, 2) == (3*4 + 2));
	
	return 0;
}
