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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <cmath>
#include "color.h"
#include "geometry.h"
#include "shape.h"

struct BRDF {
	std::shared_ptr<Pigment> pigment;
	BRDF(std::shared_ptr<Pigment> pigment): pigment{pigment} {}
	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) = 0;
};

struct DiffusiveBRDF : BRDF {
	float reflectance;
	DiffusiveBRDF(float reflectance, std::shared_ptr<Pigment> pigment):
		reflectance{reflectance}, BRDF{pigment} {};
	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) override {
		return pigment(uv) * reflectance / M_PI;
	}
};

#endif // MATERIAL_H
