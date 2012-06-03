#include <stdexcept>
#include <sstream>

#include "common/SDLSurface.h"

namespace Common {

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

SDLSurface::SDLSurface(const SDLSurface& s)
{
	SDL_Surface* ss = const_cast<SDL_Surface*>(s.getSurface());
	mSurface = SDL_ConvertSurface(ss, ss->format,
			ss->flags);
}

SDLSurface& SDLSurface::operator=(const SDLSurface& s)
{
	if(this != &s) {
		SDL_Surface* ss = const_cast<SDL_Surface*>(s.getSurface());
		SDL_Surface* old = mSurface;
		SDL_Surface* newsurf = SDL_ConvertSurface(ss,
				ss->format,
				ss->flags);
		SDL_FreeSurface(old);
		mSurface = newsurf;
	}
	return *this;
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
	changePixelColors({ { from, to } });
}

void SDLSurface::changePixelColors(const std::map<Color, Color>& mapping)
{
	return mapPixelColor([&] (const Color& c) -> Color {
			auto it = mapping.find(c);
			if(it != mapping.end())
				return it->second;
			else
				return c;
			});
}

void SDLSurface::mapPixelColor(std::function<Color (const Color&)> mapping)
{
	int bpp = mSurface->format->BytesPerPixel;
	if(bpp != 4 && bpp != 3) {
		throw std::runtime_error("Can only change pixel color with bpp = 3 or 4");
	}
	for(int i = 0; i < mSurface->h; i++) {
		for(int j = 0; j < mSurface->w; j++) {
			Uint32 *bufp = (Uint32*)mSurface->pixels + i * mSurface->pitch / bpp + j;
			Uint8 r, g, b, a;
			SDL_GetRGBA(*bufp, mSurface->format, &r, &g, &b, &a);
			Color c = Color(r, g, b);
			Color c2 = mapping(c);
			*bufp = SDL_MapRGBA(mSurface->format, c2.r, c2.g, c2.b, a);
		}
	}
}

}


