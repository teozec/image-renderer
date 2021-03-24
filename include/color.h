#ifndef COLOR_H
#define COLOR_H

struct Color {
	float r, g, b;
    
	Color operator+(const Color other) {
		return Color{r+other.r, g+other.g, b+other.b};
	}

	Color operator*(const float c) {
		return Color{c*r, c*g, c*b};
	}

	Color operator*(const Color other) {
		return Color{other.r*r, other.g*g, other.b*b};
	}

	bool operator==(Color other);

	bool operator!=(const Color other) {
		return !(*this == other);
	}
};

bool areColorsClose(Color a, Color b, float epsilon);

#endif // COLOR_H
