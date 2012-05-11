#include <stdexcept>
#include <sstream>

#include "match/SDLSurface.h"

SDLSurface::SDLSurface(const char* filename)
{
	mSurface = IMG_Load(filename);
	if(!mSurface) {
		std::stringstream ss;
		ss << "Could not load image " << filename << ": " << SDL_GetError();
		throw std::runtime_error(ss.str());
	}
}

SDLSurface::~SDLSurface()
{
	SDL_FreeSurface(mSurface);
}

const SDL_Surface* SDLSurface::getSurface() const
{
	return mSurface;
}

SDL_Surface* SDLSurface::getSurface()
{
	return mSurface;
}

void SDLSurface::changePixelColor(const Color& from,
		const Color& to)
{
	int bpp = mSurface->format->BytesPerPixel ;
	if(bpp != 4 && bpp != 3) {
		throw std::runtime_error("Can only change pixel color with bpp = 3 or 4");
	}
	Uint32 src_color = SDL_MapRGB(mSurface->format, from.r, from.g, from.b);
	Uint32 dst_color = SDL_MapRGB(mSurface->format, to.r, to.g, to.b);
	for(int i = 0; i < mSurface->h; i++) {
		for(int j = 0; j < mSurface->w; j++) {
			Uint32 *bufp = (Uint32*)mSurface->pixels + i * mSurface->pitch / bpp + j;
			if(*bufp == src_color)
				*bufp = dst_color;
		}
	}
}


