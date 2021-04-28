#ifndef CAMERA_H
#define CAMERA_H

#include <geometry.h>

/** Camera class
 * 
 * This is the class regarding the observer looking at the scene.
 * The parameters needed are only two:
 * @param	d	The distance between observer and screen.
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @see OrthogonalCamera
 * @see	PerspectiveCamera
 */
struct Camera {
	float d, a;

	Camera(float a) : d{-1.f}, a{a};
	
	virtual Ray fireRay(float u, float v) {
		/* To be continued...
		float a[3] = {-d, 0.f, 0.f};
		float b[3] = {d, (1.f-2*u)*a, 2*v-1.f};
		*/
	};
};

/** OrthogonalCamera class
 * 
 * This is the class regarding the observer looking at the scene 
 * as if he would be very far from the screen (orthogonal projection).
 * 
 * The parameters needed are only two:
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @param	transformation	The transformation to be applied to the camera (default identity).
 * 
 * This class have a parent:
 * @see	Camera
 */
struct OrthogonalCamera : Camera{
	float a = 1.f;
	Transformation transformation{};

	Ray fireRay(float u, float v) {
		Point origin = Point{-1.f, (1.f-2*u)*a, 2*v-1.f}; //bottom-left is (0, 1, -1)
		Vec dir{1.f, 0.f, 0.f}; // origin_x is -1 so dir cancel that
		return transformation*Ray(origin, dir, tmin=1.f);
	};
};

/** PerspectiveCamera class
 * 
 * This is the class regarding the observer looking at the scene 
 * at a given distance `d`.
 * 
 * The parameters needed are only two:
 * @param	d	The distance between observer and screen (default 1.0).
 * @param	a   The aspect ratio as width over height (default 1.0).
 * @param	transformation	The transformation to be applied to the camera (default identity).
 * 
 * This class have a parent:
 * @see	Camera
 */
struct PerspectiveCamera : Camera{
	float d = 1.f, a = 1.f;
	Transformation transformation{};

	Ray fireRay(float u, float v) {
		Point origin{-d, 0.f, 0.f}; // origin_x is -d, dir cancel that
		Vec dir{d, (1.f - 2*u)*a, 2*v-1.f};
		return transformation*Ray(origin, dir, tmin=1.f)
	};
};



#endif // CAMERA_H
