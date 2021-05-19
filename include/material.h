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
#include "geometry.h"
#include "hdr-image.h"

/**
 * @brief Abstract struct for pigments.
 * 
 * @see UniformPigment
 * @see CheckeredPigment
 * @see ImagePigment
 */
struct Pigment {

	/**
	 * @brief Overloading operator(). It let you get the color in a given surface coordinates (u, v).
	 * 
	 * @param coords 
	 * @return Color 
	 */
	virtual Color operator()(Vec2D coords) = 0;
};

/**
 * @brief Struct derived by Pigment of a uniform pigment, i.e. uniform colored surface.
 * 
 * @see Pigment
 * @see CheckeredPigment
 * @see ImagePigment
 */
struct UniformPigment : public Pigment {
	Color color{};

	UniformPigment() : Pigment() {}
	UniformPigment(Color color) : Pigment(), color{color} {}

	/**
	 * @brief Overloading operator(). It let you get the color in a given surface coordinates (u, v).
	 * 
	 * @param coords 
	 * @return Color 
	 */
	virtual Color operator()(Vec2D coords) {
		return color;
	}
};

/**
 * @brief Struct derived by Pigment of a checkered pigment, i.e. like a checkerboard.
 * 
 * @see Pigment
 * @see UniformPigment
 * @see ImagePigment
 */
struct CheckeredPigment : public Pigment {
	Color c1{1.f, 1.f, 1.f};
	Color c2{0.f, 0.f, 0.f};
	int nSteps = 10;

	CheckeredPigment() : Pigment() {}
	CheckeredPigment(Color c1, Color c2, int n = 10) : Pigment(), c1{c1}, c2{c2}, nSteps{n} {}

	/**
	 * @brief Overloading operator(). It let you get the color in a given surface coordinates (u, v).
	 * 
	 * @param coords 
	 * @return Color 
	 */
	virtual Color operator()(Vec2D coords) {
		int u = floor(coords.u * nSteps);
		int v = floor(coords.v * nSteps);
		return (u%2)==(v%2) ? c1 : c2;
	}
};

/**
 * @brief Struct derived by Pigment of a pigment based on a HDR image.
 * 
 * @see Pigment
 * @see UniformPigment
 * @see CheckeredPigment
 */
struct ImagePigment : public Pigment {
	HdrImage img{};

	ImagePigment() : Pigment() {}
	ImagePigment(HdrImage img) : Pigment(), img{img} {}

	/**
	 * @brief Overloading operator(). It let you get the color in a given surface coordinates (u, v).
	 * 
	 * @param coords 
	 * @return Color 
	 */
	virtual Color operator()(Vec2D coords) {
		int col = coords.u * img.width;
        int row = coords.v * img.height;
        if (col >= img.width)
            col = img.width - 1;
        if (row >= img.height)
            row = img.height - 1;
        return img.getPixel(col, row);
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

	Material() : Material{std::make_shared<DiffusiveBRDF>(DiffusiveBRDF{}), std::make_shared<UniformPigment>(UniformPigment{Color{0.f, 0.f, 0.f}})} {}
	Material(std::shared_ptr<BRDF> brdf) : Material{brdf, std::make_shared<UniformPigment>(UniformPigment{Color{0.f, 0.f, 0.f}})} {}
	Material(std::shared_ptr<Pigment> pigment) : Material{std::make_shared<DiffusiveBRDF>(DiffusiveBRDF{}), pigment} {}
	Material(std::shared_ptr<BRDF> brdf, std::shared_ptr<Pigment> pigment) : brdf{brdf}, pigment{pigment} {}
};

#endif // MATERIAL_H
