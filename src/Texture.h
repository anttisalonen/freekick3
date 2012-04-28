#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

class Texture {
	public:
		Texture(const char* filename);
		~Texture();
		GLuint getTexture() const;
	private:
		GLuint mTexture;
};

#endif

