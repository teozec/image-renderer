/* Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini

This file is part of image-renderer.

image-renderer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

image-renderer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with image-renderer.  If not, see <https://www.gnu.org/licenses/>. */

#include "material.h"

void testPigments(){
	Color color{1.f, 2.f, 3.f};
	UniformPigment pigmentUniform{color};
	assert(pigmentUniform(Vec2D{0.f, 0.f}) == color);
	assert(pigmentUniform(Vec2D{1.f, 0.f}) == color);
	assert(pigmentUniform(Vec2D{0.f, 1.f}) == color);
	assert(pigmentUniform(Vec2D{1.f, 1.f}) == color);

	HdrImage image{2, 2};
	image.setPixel(0, 0, Color{1.f, 2.f, 3.f});
	image.setPixel(1, 0, Color{2.f, 3.f, 1.f});
	image.setPixel(0, 1, Color{2.f, 1.f, 3.f});
	image.setPixel(1, 1, Color{3.f, 2.f, 1.f});

	ImagePigment pigmentImage{image};
	assert(pigmentImage(Vec2D{0.f, 0.f}) == (Color{1.f, 2.f, 3.f}));
	assert(pigmentImage(Vec2D{1.f, 0.f}) == (Color{2.f, 3.f, 1.f}));
	assert(pigmentImage(Vec2D{0.f, 1.f}) == (Color{2.f, 1.f, 3.f}));
	assert(pigmentImage(Vec2D{1.f, 1.f}) == (Color{3.f, 2.f, 1.f}));

	Color color1{1.f, 2.f, 3.f};
	Color color2{10.f, 20.f, 30.f};
	CheckeredPigment pigmentCheckered{color1, color2, 2};
	assert(pigmentCheckered(Vec2D{.25f, .25f}) == color1);
	assert(pigmentCheckered(Vec2D{.75f, .25f}) == color2);
	assert(pigmentCheckered(Vec2D{.25f, .75f}) == color2);
	assert(pigmentCheckered(Vec2D{.75f, .75f}) == color1);
}

int main()
{
	testPigments();
	return 0;
}
