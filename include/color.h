#ifndef COLOR_H
#define COLOR_H

struct Color {
	float r, g, b;
    Color operator+(Color c2);
};

#endif // COLOR_H
