#include <stdio.h>
#include <sstream>
#include <stdexcept>

#include <SDL_image.h>
#include <GL/gl.h>

#include "common/Texture.h"

namespace Common {

Texture::Texture(const SDLSurface& surf, unsigned int startrow, unsigned int height)
{
	setupSDLSurface(surf.getSurface(), startrow, height);
}

Texture::Texture(const char* filename, unsigned int startrow, unsigned int height)
{
	SDLSurface surf(filename);
	setupSDLSurface(surf.getSurface(), startrow, height);
}

Texture::Texture(const SDL_Surface* surf, unsigned int startrow, unsigned int height)
{
	setupSDLSurface(surf, startrow, height);
}

void Texture::setupSDLSurface(const SDL_Surface* surf, unsigned int startrow, unsigned int height)
{
	bool hasAlpha = surf->format->BytesPerPixel == 4;
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, surf->format->BytesPerPixel, surf->w, height ? height : surf->h,
			0, hasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
			(char*)surf->pixels + startrow * surf->w * surf->format->BytesPerPixel);
}

Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
}

GLuint Texture::getTexture() const
{
	return mTexture;
}

}
