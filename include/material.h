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
#include "random.h"
#include "camera.h"

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
	Color c2{0.5f, 0.5f, 0.5f};
	int nSteps = 10;

	CheckeredPigment() : Pigment() {}
	CheckeredPigment(Color c1, Color c2, int n = 10) : Pigment(), c1{c1}, c2{c2}, nSteps{n} {}

	/**
	 * @brief Overloading operator(). It let you get the color in a given surface coordinates pair (u, v).
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

/**
 * @brief Abstract class for a generic BSDF.
 * 
 */
struct BRDF {
	std::shared_ptr<Pigment> pigment;
	BRDF(): BRDF{UniformPigment{}} {}
	template <class T> BRDF(const T &pigment): pigment{std::make_shared<T>(pigment)} {}
	
	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) = 0;
	virtual Ray scatterRay(PCG &pcg, Vec incomingDir, Point interactionPoint, Normal normal, int depth) = 0;

	/**
	 * @brief Returns a scattered direction.
	 * 
	 * @param normal 
	 * @param cosThetaSq Squared cosine of angle between incoming ray and normal.
	 * @param phi 
	 * @return Vec
	 */
	Vec scatter(Normal normal, float cosThetaSq, float phi){
		ONB onb{normal};
		float cosTheta = sqrt(cosThetaSq);
		float sinTheta = sqrt(1.f - cosThetaSq);
		return onb.e1*cos(phi)*cosTheta + onb.e2*sin(phi)*cosTheta + onb.e3*sinTheta;
	}

	/**
	 * @brief Returns the reflected direction.
	 * 
	 * @return Vec
	 */
	Vec reflect(Vec dir, Normal normal){
		return dir - normal.toVec()*(normal.toVec().dot(dir)*2);
	}

	/**
	 * @brief Returns the scattered direction given the index-of-refraction ratio.
	 * 
	 * @param dir 
	 * @param normal 
	 * @param refractionRatio 
	 * @param inward 
	 * @return Vec 
	 */
	Vec refract(Vec dir, Normal normal, float refractionRatio, bool inward){
		float cos = normal.toVec().dot(-dir);
		Vec outDirPerpendicular = (dir - normal.toVec()*cos)*refractionRatio;
		Vec outDirParallel = - normal.toVec()*sqrt(1.f - refractionRatio*refractionRatio*(1.f - cos*cos));
		return outDirPerpendicular + outDirParallel;
	}

};

struct DiffusiveBRDF : BRDF {
	float reflectance;
	DiffusiveBRDF(float reflectance = 1.f):
		reflectance{reflectance}, BRDF() {};
	template <class T> DiffusiveBRDF(const T &pigment):
		reflectance{1.f}, BRDF(pigment) {};
	template <class T> DiffusiveBRDF(float reflectance, const T &pigment):
		reflectance{reflectance}, BRDF(pigment) {};
	
	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) override {
		return (*pigment)(uv) * (reflectance / M_PI);
	}

	virtual Ray scatterRay(PCG &pcg, Vec incomingDir, Point interactionPoint, Normal normal, int depth) override {
		float cosThetaSq = pcg.randFloat();
		float phi = 2.f * M_PI * pcg.randFloat();

		return Ray{interactionPoint, scatter(normal, cosThetaSq, phi), depth, 1e-5f};
	}
};

struct SpecularBRDF : BRDF {
	float thresholdAngle = M_PI/1800.f;
	float roughness = 0.f;
	SpecularBRDF(float roughness, float thresholdAngle):
		thresholdAngle{thresholdAngle}, roughness{roughness}, BRDF() {};
	template <class T> SpecularBRDF(float roughness, const T &pigment):
		roughness{roughness}, BRDF(pigment) {};
	template <class T> SpecularBRDF(float roughness, float thresholdAngle, const T &pigment):
		thresholdAngle{thresholdAngle}, roughness{roughness}, BRDF(pigment) {};

	virtual Color eval(Normal normal, Vec in, Vec out, Vec2D uv) override {
		float thetaIn = acos(normal.toVec().dot(in));
		float thetaOut = acos(normal.toVec().dot(out));
		if (abs(thetaIn - thetaOut) < thresholdAngle)
			return (*pigment)(uv);
		else
			return BLACK;
	}

	virtual Ray scatterRay(PCG &pcg, Vec incomingDir, Point interactionPoint, Normal n, int depth) override {
		return Ray{interactionPoint, reflect(incomingDir, n) + pcg.randDir(n)*roughness, depth, 1e-5f};
	}
};
/*
struct DielectricBSDF : BRDF {

}*/

struct Material {
	std::shared_ptr<BRDF> brdf;
	std::shared_ptr<Pigment> emittedRadiance;

	Material() : Material{DiffusiveBRDF{}, UniformPigment{BLACK}} {}
	template <class B> Material(const B &brdf) : Material{brdf, UniformPigment{BLACK}} {}
	//template <class P> Material(const P &emittedRadiance) : Material{DiffusiveBRDF{}, emittedRadiance} {} // Cannot have both Material(brdf) and Material(emittedRadiance) using templates
	template <class B, class P> Material(const B &brdf, const P &emittedRadiance) : brdf{std::make_shared<B>(brdf)}, emittedRadiance{std::make_shared<P>(emittedRadiance)} {}
};

#endif // MATERIAL_H
