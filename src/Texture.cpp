#include <stdio.h>
#include <sstream>
#include <stdexcept>

#include <SDL_image.h>
#include <GL/gl.h>

#include "Texture.h"

Texture::Texture(const char* filename, unsigned int startrow, unsigned int height)
{
	SDL_Surface* surf = IMG_Load(filename);
	if(!surf) {
		std::stringstream ss;
		ss << "Could not load image " << filename << ": " << SDL_GetError();
		throw std::runtime_error(ss.str());
	}
	bool hasAlpha = surf->format->BytesPerPixel == 4;
	printf("%s has %d bpp\n", filename, surf->format->BytesPerPixel);
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, surf->format->BytesPerPixel, surf->w, height ? height : surf->h,
			0, hasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
			(char*)surf->pixels + startrow * surf->w * surf->format->BytesPerPixel);
	SDL_FreeSurface(surf);
}

Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
}

GLuint Texture::getTexture() const
{
	return mTexture;
}


