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

#ifndef CAMERA_H
#define CAMERA_H

#include <limits>
#include "geometry.h"
#include "hdr-image.h"
#include "color.h"
#include "random.h"

struct Ray {
	Point origin;
	Vec dir;
	float tmin, tmax;
	int depth;

	Ray(Point origin = Point(), Vec dir = Vec(), int depth = 0, float tmin = 1e-5, float tmax = std::numeric_limits<float>::infinity()):
		origin{origin}, dir{dir}, depth{depth}, tmin{tmin}, tmax{tmax} {}

	bool isClose(Ray other, float epsilon) {
		return areClose<Point>(origin, other.origin, epsilon) and
			areClose<Vec>(dir, other.dir, epsilon);
	}

	bool operator==(Ray other) {
		return origin == other.origin and dir == other.dir;
	}

	bool operator!=(Ray other) {
		return !(*this == other);
	}
	
	Ray operator=(const Ray &other) {
		origin = other.origin;
		dir = other.dir;
		tmin = other.tmin;
		tmax = other.tmax;
		depth = other.depth;
		return *this;
	}

	/**
	 * @brief Return the Point corresponding to the Ray at the affine parameter t
	 *
	 * @param t The value of the affine parameter to evaluate the Ray at
	 * @return Point
	 */
	Point operator()(float t) {
		return origin + dir * t;
	}
};

Ray operator*(Transformation tr, Ray ray) {
	return Ray(tr * ray.origin, tr * ray.dir, ray.depth, ray.tmin, ray.tmax);
}

/** Camera class
 * 
 * @brief This is the class regarding the observer looking at the scene.
 * 
 * @param	a	The aspect ratio as width over height (default 1.0).
 * @param	d   The distance between observer and screen (default -1.0).
 * 
 * @see OrthogonalCamera
 * @see	PerspectiveCamera
 */
struct Camera {
	float a, d;

	Camera(float a = 1.f, float d = -1.f) : a{a}, d{d} {}
	
	/**
	 * @brief Fires a ray at a given position on the screen.
	 * 
	 * @param u Float value between 0 and 1 corresponding to x coordinate.
	 * @param v Float value between 0 and 1 corresponding to y coordinate.
	 * 
	 * @return Ray
	 * 
	 * @see Camera
	 * @see Ray
	 */
	virtual Ray fireRay(float u, float v) = 0;
};

/** OrthogonalCamera class
 * 
 * @brief This is the class regarding the observer looking at the scene 
 * as if he would be very far from the screen (orthogonal projection).
 * 
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @param	transformation	The transformation to be applied to the camera (default identity).
 * 
 * @see	Camera
 */
struct OrthogonalCamera : Camera {
	Transformation transformation{};

	OrthogonalCamera(float a, Transformation t) : Camera(a), transformation{t} {}
	OrthogonalCamera(float a) : Camera(a) {}
	OrthogonalCamera(Transformation t) : transformation{t} {}
	OrthogonalCamera() {}

	virtual Ray fireRay(float u, float v) {
		Point origin{-1.f, (1.f-2*u)*a, 2*v-1.f}; //bottom-left is (0, 1, -1)
		Vec dir{1.f, 0.f, 0.f}; // origin_x is -1 so dir cancel that
		return transformation*Ray(origin, dir, 0, 1.f);
	}
};

/** PerspectiveCamera class
 * 
 * @brief This is the class regarding the observer looking at the scene 
 * at a given distance `d`.
 * 
 * @param	a	The aspect ratio as width over height (default 1.0).
 * @param	d   The distance between observer and screen (default -1.0).
 * @param	transformation	The transformation to be applied to the camera (default identity).
 * 
 * @see	Camera
 */
struct PerspectiveCamera : Camera {
	Transformation transformation{};

	PerspectiveCamera(float a, float d, Transformation t) : Camera(a, d), transformation{t} {}
	PerspectiveCamera(float a, Transformation t, float d = 1.f) : Camera(a, d), transformation{t} {}
	PerspectiveCamera(float a, float d = 1.f) : Camera(a, d) {}
	PerspectiveCamera(Transformation t) : transformation{t} {}

	virtual Ray fireRay(float u, float v) {
		Point origin{-d, 0.f, 0.f}; // origin_x is -d, dir cancel that
		Vec dir{d, (1.f - 2*u)*a, 2*v-1.f};
		return transformation*Ray(origin, dir, 0, 1.f);
	}
};

/**
 * @brief Tracer of the scene. 
 * Given the image and the camera it fires rays through each pixel.
 * If `samplesPerSide` is given (not zero) stratified sampling is applied.
 */
struct ImageTracer {
	HdrImage &image;
	Camera &camera;
	int samplesPerSide;
	PCG pcg{};

	ImageTracer(HdrImage &image, Camera &camera): 
		image{image}, camera{camera}, samplesPerSide{} {}

	ImageTracer(HdrImage &image, Camera &camera, int samples): 
		image{image}, camera{camera}, samplesPerSide{samples} {}

	/**
	 * @brief Return a Ray starting from the observer and passing through the screen at (col, row)
	 *
	 * @param col The column of the intersected pixel on the screen
	 * @param row The row of the intersected pixel on the screen
	 * @param uPixel The u coordinate of the intersected point on the pixel surface
	 * @param vPixel The v coordinate of the intersected point on the pixel surface
	 * @return Ray
	 */
	Ray fireRay(int col, int row, float uPixel = .5f, float vPixel = .5f) {
		float u = (col + uPixel) / image.width;
		float v = 1.f - (row + vPixel) / image.height;
		return camera.fireRay(u, v);
	}

	/**
	 * @brief Write the scene to the image, calculating the color for each pixel using the color function
	 *
	 * @tparam T	The signature of the color function
	 * @param color The function to compute a Color given a Ray
	 */
	template <typename T> void fireAllRays(T colorFunc) {
		for (int row{}; row < image.height; row++) {
			for (int col{}; col < image.width; col++) {
				Color cumColor{0.f, 0.f, 0.f};
				if (samplesPerSide > 0) {
					for (int rowPixel{}; rowPixel<samplesPerSide; rowPixel++) {
						for (int colPixel{}; colPixel<samplesPerSide; colPixel++) {
							float uPixel = (colPixel+pcg.randFloat())/samplesPerSide;
							float vPixel = (rowPixel+pcg.randFloat())/samplesPerSide;
							Ray ray = fireRay(col, row, uPixel, vPixel);
							cumColor += colorFunc(ray);
						}
					}
					image.setPixel(col, row, cumColor);
				} else {
					Ray ray = fireRay(col, row);
					Color color = colorFunc(ray);
					image.setPixel(col, row, color);
				}
			}
		}
	}
};

#endif // CAMERA_H
