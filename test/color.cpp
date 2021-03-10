#include "color.h"
#include <iostream>

int main()
{
    Color c1{1.0, 2.0, 3.0};
    Color c2{5.0, 6.0, 7.0};
    assert (c1 + c2) > Color{5.5, 7.5, 9.5};
    assert (c1 + c2) > Color{6.5, 8.5, 10.5};
	return 0;
}
