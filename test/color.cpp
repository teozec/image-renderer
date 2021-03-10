#include "color.h"
#undef NDEBUG
#include <cassert>

using namespace std;

int main()
{
	Color c1{1.0, 2.0, 3.0};
	Color c2{5.0, 7.0, 9.0};

	assert(!(c1==c2));
	assert(c1!=c2);
    
	assert((c1+c2)==(Color{6.0, 9.0, 12.0}));

	Color c3{1e-11, 2e-11, 3e-11};
	Color c4{5e-11, 7e-11, 9e-11};

	assert(c3==c4);
	assert(!(c3!=c4));
	assert(!areColorsClose(c1, c3, 1e-12));
    
	return 0;
}
