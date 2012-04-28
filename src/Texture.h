#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

class Texture {
	public:
		Texture(const char* filename, unsigned int startrow = 0,
				unsigned int height = 0);
		~Texture();
		GLuint getTexture() const;
	private:
		GLuint mTexture;
};

#endif

