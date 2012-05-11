#ifndef COMMON_SDL_UTILS_H
#define COMMON_SDL_UTILS_H

#include <SDL.h>

namespace Common {

	class SDL_utils {
		public:
			static SDL_Surface* initSDL(int w, int h);
			static void setupOrthoScreen(int w, int h);
			static const char* GLErrorToString(GLenum err);
	};

}

#endif

