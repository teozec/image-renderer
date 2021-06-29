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

#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>
#include "geometry.h"

/**
 * @brief A random number generator using the PCG algorithm.
 * 
 * @param initState	The seed of the generator
 * @param initSeq	The sequence identifier, to ensure that orthogonal sequences are generated.
 */
struct PCG {
	uint64_t state = 0, inc;
	PCG(uint64_t initState = 42, uint64_t initSeq = 54): inc{(initSeq << 1) | 1} {
		(void) (*this)();
		state += initState;
		(void) (*this)();
	}

	uint32_t operator()() {
		uint64_t oldState = state;
		state = oldState * 6364136223846793005 + inc;
		uint32_t xorShifted = ((oldState >> 18) ^ oldState) >> 27;
		uint32_t rot = oldState >> 59;
		return (xorShifted >> rot) | (xorShifted << ((-rot) & 31));
	}

	float randFloat(){
		//Return a random float (uniform distribution) in [0,1]
		return (float)(*this)() / UINT32_MAX;
	}

	Vec randDir(Normal normal) {
		ONB onb{normal};
		float cosThetaSq = randFloat();
		float cosTheta = std::sqrt(cosThetaSq);
		float sinTheta = std::sqrt(1.f - cosThetaSq);
		float phi = 2.f * M_PI * randFloat();
		return onb.e1*cos(phi)*cosTheta + onb.e2*sin(phi)*cosTheta + onb.e3*sinTheta;
	}
};


#endif // RANDOM_H
