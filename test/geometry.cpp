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
#include "random.h"
#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

bool areClose(float a, float b, float epsilon = 1e-5)
{
	return abs(a-b) < epsilon;
}

void testONB()
{
	PCG pcg;
	for (int i{}; i < 10000000; i++) {
		Normal normal{pcg.randFloat(), pcg.randFloat(), pcg.randFloat()};
		ONB onb{normal};

		assert((onb.e3 == normal.toVec().versor()));

		assert(areClose(onb.e1.dot(onb.e2), 0.f));
		assert(areClose(onb.e2.dot(onb.e3), 0.f));
		assert(areClose(onb.e3.dot(onb.e1), 0.f));

		assert(areClose(onb.e1.squaredNorm(), 1.f));
		assert(areClose(onb.e2.squaredNorm(), 1.f));
		assert(areClose(onb.e3.squaredNorm(), 1.f));
	}
}

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

	assert(_areClose(a.squaredNorm(), 14.f, epsilon));
	assert(_areClose(a.norm()*a.norm(), 14.f, bigeps));
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

	// Test Transformation constructor and Transform::isConsistent() method
	float mat1[4][4]={	{1.f, 2.f, 3.f, 4.f}, 
						{5.f, 6.f, 7.f, 8.f}, 
						{9.f, 9.f, 8.f, 7.f}, 
						{6.f, 5.f, 4.f, 1.f}};
	float matInv1[4][4]={{-3.75, 2.75, -1.f, 0.f},
						{4.375, -3.875, 2.f, -0.5},
						{0.5, 0.5, -1.f, 1.f},
						{-1.375, 0.875, 0.f, -0.5}};
	Transformation m1{mat1,matInv1};
	assert(m1.isConsistent());
	Transformation copy1{m1.m, m1.mInv};
	assert(m1==copy1);
	Transformation copy2{m1.m, m1.mInv};
	copy2.m[2][2] += 1.f;
	assert(copy2!=m1);
	assert(!(copy2.isConsistent()));
	Transformation copy3{m1.m, m1.mInv};
	copy3.mInv[2][2] += 1.f;
	assert(copy3!=m1);
	assert(!(copy3.isConsistent()));

	// Test multiplication methods
	float mat2[4][4]={	{3.0, 5.0, 2.0, 4.0}, 
						{4.0, 1.0, 0.0, 5.0}, 
						{6.0, 3.0, 2.0, 0.0}, 
						{1.0, 4.0, 2.0, 1.0}};
	float matInv2[4][4]={{0.4, -0.2, 0.2, -0.6},
						{2.9, -1.7, 0.2, -3.1},
						{-5.55, 3.15, -0.4, 6.45},
						{-0.9, 0.7, -0.2, 1.1}};
	Transformation m2{mat2, matInv2};
	assert(m2.isConsistent());
	float matProd[4][4]={{33.f, 32.f, 16.f, 18.f}, 
						{89.f, 84.f, 40.f, 58.f}, 
						{118.f, 106.f, 48.f, 88.f}, 
						{63.f, 51.f, 22.f, 50.f}};
	float matProdInv[4][4]={{-1.45, 1.45, -1.f, 0.6},
						{-13.95, 11.95, -6.5, 2.6},
						{25.525, -22.025, 12.25, -5.2},
						{4.825, -4.325, 2.5, -1.1}};
	Transformation p{matProd, matProdInv};
	assert(p.isConsistent(1e-4));
	assert(p.areTransformationsClose(m1*m2, 1e-4));

	float mat3[4][4]={	{1.f, 2.f, 3.f, 4.f}, 
						{5.f, 6.f, 7.f, 8.f}, 
						{9.f, 9.f, 8.f, 7.f}, 
						{0.f, 0.f, 0.f, 1.f}};
	float matInv3[4][4]={{-3.75, 2.75, -1.f, 0.f},
						{5.75, -4.75, 2.f, 1.f},
						{-2.25, 2.25, -1.f, -2.f},
						{0.f, 0.f, 0.f, 1.f}};
	Transformation m3{mat3, matInv3};
	assert(m3.isConsistent());
	Vec resVec{14.f, 38.f, 51.f};
	assert((m3*Vec{1.f, 2.f, 3.f}) == resVec);
	Point resPoint{18.f, 46.f, 58.f};
	assert((m3*Point{1.f, 2.f, 3.f}) == resPoint);
	Normal resNormal{-8.75, 7.75, -3.f};
	assert((m3*Normal{3.f, 2.f, 4.f}) == resNormal);

	// Test inverse method
	Transformation I, m1Inv = m1.inverse();
	assert(m1Inv.isConsistent());
	assert((m1Inv*m1) == I);

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

	testONB();

	return 0;
}
