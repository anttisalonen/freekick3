#ifndef SDLSURFACE_H
#define SDLSURFACE_H

#include <SDL_image.h>

#include "common/Color.h"

namespace Common {

class SDLSurface {
	public:
		SDLSurface(const char* filename);
		~SDLSurface();
		const SDL_Surface* getSurface() const;
		SDL_Surface* getSurface();
		void changePixelColor(const Color& from,
				const Color& to);
	private:
		SDL_Surface* mSurface;
};

}

#endif
