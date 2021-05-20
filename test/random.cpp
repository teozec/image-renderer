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

#include "random.h"
#undef NDEBUG
#include <cassert>

int main()
{
	PCG pcg{};
	assert(pcg.state == 1753877967969059832); // FAILS
	assert(pcg.inc == 109);
	uint32_t expected[6] = {2707161783, 2068313097, 3122475824, 2211639955, 3215226955, 3421331566};
	for (int i{}; i<6; i++){	// FAILS
		uint32_t result = pcg();
		assert(expected[i] == result);
	}
	return 0;
}
