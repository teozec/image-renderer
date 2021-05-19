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

#ifndef RENDERER_H
#define RENDERER_H

#include "shape.h"


struct Renderer {
	World world;
	Color backgroundColor, color;

	Renderer() {}
	Renderer(World w) : world{w} {}
	Renderer(World w, Color c = Color{1.f, 1.f, 1.f}, Color bg = Color{0.f, 0.f, 0.f}) : world{w}, color{c}, backgroundColor{bg} {}

	virtual Color render(Ray ray) = 0;
};

struct OnOffRenderer : public Renderer {

	OnOffRenderer() : Renderer() {}
	OnOffRenderer(World w, Color c = Color{1.f, 1.f, 1.f}, Color bg = Color{0.f, 0.f, 0.f}) : Renderer(w, c, bg) {}

	virtual Color render(Ray ray) {
		return world.rayIntersection(ray).hit ? color : backgroundColor;
	}
};

struct FlatRenderer : public Renderer {

	FlatRenderer() : Renderer() {}
	FlatRenderer(World w, Color c = Color{1.f, 1.f, 1.f}, Color bg = Color{0.f, 0.f, 0.f}) : Renderer(w, c, bg) {}

	virtual Color render(Ray ray) {
	    HitRecord record = world.rayIntersection(ray);
		return record.hit ? record.shape->material.brdf->pigment->operator()(record.surfacePoint) : backgroundColor;
	}
};

#endif //   RENDERER_H