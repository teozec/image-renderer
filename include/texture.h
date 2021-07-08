/**
 *  Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini
 * 
 * This file is part of image-renderer.
 * 
 * image-renderer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * image-renderer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with image-renderer.  If not, see <https://www.gnu.org/licenses/>. 
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "noise.h"
#include "hdr-image.h"

/* USAGE EXAMPLE
	Wood texture{1000, WHITE, 20.f};
	HdrImage image = texture.getImage();
	ofstream outPfm;
	outPfm.open("texture.pfm");
	image.writePfm(outPfm);
	outPfm.close();
*/

struct Texture {
	int dim;
	Color baseColor;
	PerlinNoise pn;
	float scale;


	Texture(int dim = 1000, float scale = 1.f, Color c = WHITE) : dim{dim}, baseColor{c}, scale{scale} {}

	virtual HdrImage getImage() = 0;
};

struct Marble : public Texture {
	Marble() : Texture() {}
	Marble(int dim) : Texture(dim) {}
	Marble(int dim, Color c, float scale = 5.f) : Texture(dim, scale, c) {}

	virtual HdrImage getImage() override {
		HdrImage img{dim, dim};
		for(unsigned int i = 0; i < dim; ++i) {     // y
			for(unsigned int j = 0; j < dim; ++j) {  // x
				float x = (float)j/((float)dim);
				float y = (float)i/((float)dim);
				float n = 0.5f*(1.f + sinf((0.8f + scale*pn.turb(x, y, 1.f))*2.f*M_PI));
				n -= floor(n);
				img.setPixel(i, j, baseColor*n);
			}
		}
		return img;
	}
};

struct Noise : public Texture {
	Noise() : Texture() {}
	Noise(int dim) : Texture(dim) {}
	Noise(int dim, Color c, float scale = 10.f) : Texture(dim, scale, c) {}

	virtual HdrImage getImage() override {
		HdrImage img{dim, dim};
		for(unsigned int i = 0; i < dim; ++i) {     // y
			for(unsigned int j = 0; j < dim; ++j) {  // x
				float x = (float)j/((float)dim);
				float y = (float)i/((float)dim);
				float n = pn.noise(scale * x, scale * y, scale*0.8f);
				n -= floor(n);
				img.setPixel(i, j, baseColor*n);
			}
		}
		return img;
	}
};

struct Turbolence : public Texture {
	Turbolence() : Texture() {}
	Turbolence(int dim) : Texture(dim) {}
	Turbolence(int dim, Color c, float scale = 10.f) : Texture(dim, scale, c) {}

	virtual HdrImage getImage() override {
		HdrImage img{dim, dim};
		for(unsigned int i = 0; i < dim; ++i) {     // y
			for(unsigned int j = 0; j < dim; ++j) {  // x
				float x = (float)j/((float)dim);
				float y = (float)i/((float)dim);
				float n = pn.turb(scale * x, scale * y, scale*0.8f);
				n -= floor(n);
				img.setPixel(i, j, baseColor*n);
			}
		}
		return img;
	}
};

struct Wood : public Texture {
	Wood() : Texture() {}
	Wood(int dim) : Texture(dim) {}
	Wood(int dim, Color c, float scale = 5.f) : Texture(dim, scale, c) {}

	virtual HdrImage getImage() override {
		HdrImage img{dim, dim};
		for(unsigned int i = 0; i < dim; ++i) {     // y
			for(unsigned int j = 0; j < dim; ++j) {  // x
				float x = (float)j/((float)dim);
				float y = (float)i/((float)dim);
				float n = scale * pn.noise(x, y, 0.8f);
				n -= floor(n);
				img.setPixel(i, j, baseColor*n);
			}
		}
		return img;
	}
};

#endif //TEXTURE_H