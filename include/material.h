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
#include "hdr-image.h"
#include "shape.h"

struct Pigment {
	virtual Color operator()(Vec2D coords) = 0;
};

struct UniformPigment : public Pigment {
	Color color{};

	UniformPigment() : Pigment() {}
	UniformPigment(Color color) : Pigment(), color{color} {}

	Color operator()(Vec2D coords) {
		return color;
	}
};

struct BRDF {
	std::shared_ptr<Pigment> pigment;
	BRDF(std::shared_ptr<Pigment> pigment = std::make_shared<UniformPigment>(UniformPigment{})): pigment{pigment} {}
	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) = 0;
};

struct DiffusiveBRDF : BRDF {
	float reflectance;
	DiffusiveBRDF(float reflectance = 1.f):
		reflectance{reflectance}, BRDF() {};
	DiffusiveBRDF(std::shared_ptr<Pigment> pigment):
		reflectance{1.f}, BRDF(pigment) {};
	DiffusiveBRDF(float reflectance, std::shared_ptr<Pigment> pigment):
		reflectance{reflectance}, BRDF(pigment) {};
	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) override {
		return (*pigment)(uv) * (reflectance / M_PI);
	}
};

struct Material {
	std::shared_ptr<BRDF> brdf;
	std::shared_ptr<Pigment> pigment;

	Material(): Material{std::make_shared<DiffusiveBRDF>(DiffusiveBRDF{}), std::make_shared<UniformPigment>(UniformPigment{Color{0.f, 0.f, 0.f}})} {}
	Material(std::shared_ptr<BRDF> brdf): Material{brdf, std::make_shared<UniformPigment>(UniformPigment{Color{0.f, 0.f, 0.f}})} {}
	Material(std::shared_ptr<Pigment> pigment): Material{std::make_shared<DiffusiveBRDF>(DiffusiveBRDF{}), pigment} {}
	Material(std::shared_ptr<BRDF> brdf, std::shared_ptr<Pigment> pigment): brdf{brdf}, pigment{pigment} {}
};

#endif // MATERIAL_H
