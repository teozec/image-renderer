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

#include "geometry.h"
#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

int main()
{
	float epsilon = 1e-10f;
	float bigeps = 1e-5f;

	// Test Vec
	Vec a{1.f, 2.f, 3.f};
	Vec b{4.f, 6.f, 8.f};

	assert(a == a);
	assert(!(a == b));
	assert(a != b);
	assert(areClose<Vec>(a, b, 10.f));

	assert((a + b) == (Vec{5.f, 8.f, 11.f}));
	assert((b - a) == (Vec{3.f, 4.f, 5.f}));
	assert((a * 2.f) == (Vec{2.f, 4.f, 6.f}));

	assert(_areClose(a.dot(b), 40.f, epsilon));
	assert(a.cross(b) == (Vec{-2.f, 4.f, -2.f}));
	assert(b.cross(a) == (Vec{2.f, -4.f, 2.f}));

	//cout << a.norm() <<endl; 
	//cout << sqrt(a.squaredNorm()) <<endl;

	assert(_areClose(a.squaredNorm(), 14.f, epsilon));
	assert(_areClose(a.norm()*a.norm(), 14.f, bigeps)); // bigeps = 1e-5
	assert(a.versor() == (Vec{a.x/sqrt(14.f), a.y/sqrt(14.f), a.z/sqrt(14.f)}));

	b = a;
	b.normalize();
	assert(b == (Vec{a.x/sqrt(14.f), a.y/sqrt(14.f), a.z/sqrt(14.f)}));

	// Test Point
	Point Pa{1.f, 2.f, 3.f};
	Point Pb{3.f, 6.f, 9.f};

	assert(Pa==Pa);
	assert(!(Pa==Pb));

	assert((Pa * 3) == Pb);
	assert((Pa + Pb) == (Point{4.f, 8.f, 12.f}));
	assert((Pb - Pa) == (Vec{2.f, 4.f, 6.f}));
	assert((Pa + Vec{2.f, 4.f, 6.f}) == Pb);


	// Test transformation functions
	Transformation tr1 = translation(Vec{1.f, 2.f, 3.f});
	assert(tr1.isConsistent());
	Transformation tr2 = translation(Vec{4.f, 6.f, 8.f});
	assert(tr2.isConsistent());
	Transformation prod = tr1 * tr2;
	assert(prod.isConsistent());
	assert(prod == translation(Vec{5.f, 8.f, 11.f}));

	assert(rotationX(0.1).isConsistent());
	assert(rotationY(0.1).isConsistent());
	assert(rotationZ(0.1).isConsistent());
	Vec vx{1.f, 0.f, 0.f}, vy{0.f, 1.f, 0.f}, vz{0.f, 0.f, 1.f};
	assert(areClose<Vec>(rotationX(M_PI/2) * vy, vz, bigeps));
	assert(areClose<Vec>(rotationY(M_PI/2) * vz, vx, bigeps));
	assert(areClose<Vec>(rotationZ(M_PI/2) * vx, vy, bigeps));

	Transformation s1 = scaling(2.f, 5.f, 10.f);
	assert(s1.isConsistent());
	Transformation s2 = scaling(3.f, 2.f, 4.f);
	assert(s2.isConsistent());
	assert(s1 * s2 == scaling(6.f, 10.f, 40.f));

	return 0;
}
