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

#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <memory>
#include <vector>

struct Shape {};
struct Sphere : public Shape {};
struct Plane : public Shape {};

/** World class
 * @brief This is the class containing all the shapes of the scene.
 * 
 * @param shapes[]	List of shapes.
 */
struct World {
	std::vector<std::shared_ptr<Shape>> shapes;

	void add(Shape newShape){
		shapes.push_back(std::make_shared<Shape> (newShape));
	}

	virtual rayIntersection(Ray ray) -> auto HitRecord {
		closest = NULL;

		for(int i{}; i<len(shapes); i++){
			intersection = shapes[i].rayIntersection(ray);
			if(!intersection){
				continue;
			}
			if((!closest)||(intersection.t < closest.t){
				closest = intersection;
			}
		}

		return closest;
	}
};

#endif // #SHAPE_H