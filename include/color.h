#ifndef COLOR_H
#define COLOR_H

struct Color {
	float r, g, b;
    
    Color operator+(const Color other);
	bool operator==(const Color other);
	bool operator!=(const Color other);
};

bool areColorsClose(const Color a, const Color b, float epsilon);

#endif // COLOR_H
