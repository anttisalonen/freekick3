#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

#include "common/SDLSurface.h"

namespace Common {

class Texture {
	public:
		Texture(const SDLSurface& surf, unsigned int startrow = 0,
				unsigned int height = 0);
		Texture(const SDL_Surface* surf, unsigned int startrow = 0,
				unsigned int height = 0);
		Texture(const char* filename, unsigned int startrow = 0,
				unsigned int height = 0);
		~Texture();
		GLuint getTexture() const;
	private:
		void setupSDLSurface(const SDL_Surface* s,
				unsigned int startrow, unsigned int height);
		GLuint mTexture;
};

}

#endif

