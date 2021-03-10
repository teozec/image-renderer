#include "color.h"
#include <cstdlib>

Color Color::operator+(Color c2){
    Color colorSum = c2;
    colorSum.r += r;
    colorSum.g += g;
    colorSum.b += b;
}
