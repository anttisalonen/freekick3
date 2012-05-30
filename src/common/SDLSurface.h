#ifndef SDLSURFACE_H
#define SDLSURFACE_H

#include <map>

#include <SDL_image.h>

#include "common/Color.h"

namespace Common {

class SDLSurface {
	public:
		SDLSurface(const char* filename);
		SDLSurface(const SDLSurface& s);
		~SDLSurface();
		SDLSurface& operator=(const SDLSurface& s);
		const SDL_Surface* getSurface() const;
		SDL_Surface* getSurface();
		void changePixelColor(const Color& from,
				const Color& to);
		void changePixelColors(const std::map<Color, Color>& mapping);
	private:
		SDL_Surface* mSurface;
};

}

#endif
