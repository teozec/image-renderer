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

#include <algorithm>
#include "shape.h"
#include "color.h"

struct Renderer {
	World world;
	Color backgroundColor;

	Renderer() {}
	Renderer(World w) : world{w} {}
	Renderer(World w, Color bg = BLACK) : world{w}, backgroundColor{bg} {}

	virtual Color operator()(Ray ray) = 0;
};

/**
 * @brief Rendering algorithm based on a simple hit (white) or miss (black) method.
 * 
 */
struct OnOffRenderer : public Renderer {
	Color color;
	OnOffRenderer() : Renderer() {}
	OnOffRenderer(World w, Color c = WHITE, Color bg = BLACK) : Renderer(w, bg), color{c} {}

	/**
	* @brief Given a ray returns white if the ray hits the shape surface, the background color otherwise.
	* 
	* @param ray 
	* @return Color 
	*/
	virtual Color operator()(Ray ray) override {
		return world.rayIntersection(ray).hit ? color : backgroundColor;
	}
};

/**
 * @brief Rendering algorithm based on a flat material surface.
 * 
 */
struct FlatRenderer : public Renderer {

	FlatRenderer() : Renderer() {}
	FlatRenderer(World w, Color bg = BLACK) : Renderer(w, bg) {}

	/**
	* @brief Given a ray returns the proper shape pigment if it hits the surface, the background color otherwise.
	* 
	* @param ray 
	* @return Color 
	*/
	virtual Color operator()(Ray ray) override {
		HitRecord record = world.rayIntersection(ray);
		return record.hit ? (*record.shape->material.brdf->pigment)(record.surfacePoint) : backgroundColor;
	}
};

struct DebugRenderer : public Renderer {

	DebugRenderer() : Renderer() {}
	DebugRenderer(World w) : Renderer(w, BLACK) {}

	/**
	* @brief Given a ray returns the proper shape pigment if it hits the surface, the background color otherwise.
	* 
	* @param ray 
	* @return Color 
	*/
	virtual Color operator()(Ray ray) override {
		HitRecord record = world.rayIntersection(ray);
		return record.hit ? Color{abs(record.normal.x), abs(record.normal.y), abs(record.normal.z)} : BLACK;
	}
};

struct PathTracer : public Renderer {
	PCG pcg;
	int nRays, maxDepth, minDepth;
	PathTracer() {}
	PathTracer(World w, PCG pcg = PCG{}, int nRays = 10, int maxDepth = 2, int minDepth = 3, Color bg = BLACK) : Renderer(w, bg), pcg{pcg}, nRays{nRays}, maxDepth{maxDepth}, minDepth{minDepth} {}

	virtual Color operator()(Ray ray) override {
		if (ray.depth > maxDepth)
			return BLACK;

		HitRecord hit{world.rayIntersection(ray)};
		if (!hit.hit)
			return backgroundColor;

		Material hitMaterial{hit.shape->material};
		Color hitColor{(*hitMaterial.brdf->pigment)(hit.surfacePoint)};
		Color emittedRadiance{(*hitMaterial.emittedRadiance)(hit.surfacePoint)};

		float hitColorLum = std::max({hitColor.r, hitColor.g, hitColor.b});

		// Russian roulette
		if (ray.depth >= minDepth) {
			float q = std::max(0.05f, 1 - hitColorLum);
			if (pcg.randFloat() > q)
				hitColor /= (1.f - q);
			else
				return emittedRadiance;
		}

		// Montecarlo
		Color cumulativeRadiance = BLACK;

		if (hitColorLum > 0.f)
			for (int i{}; i < nRays; i++)
				cumulativeRadiance += hitColor * (*this)(hitMaterial.brdf->scatterRay(
					pcg, hit.ray.dir, hit.worldPoint, hit.normal, ray.depth+1));	

		return emittedRadiance + cumulativeRadiance / (float) nRays;
	}
};

#endif //   RENDERER_H
