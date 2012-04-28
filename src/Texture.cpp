#include <sstream>
#include <stdexcept>

#include <SDL_image.h>
#include <GL/gl.h>

#include "Texture.h"

Texture::Texture(const char* filename)
{
	SDL_Surface* surf = IMG_Load(filename);
	if(!surf) {
		std::stringstream ss;
		ss << "Could not load image " << filename << ": " << SDL_GetError();
		throw std::runtime_error(ss.str());
	}
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, surf->w, surf->h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, surf->pixels);
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


