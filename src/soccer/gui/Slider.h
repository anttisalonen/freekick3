#ifndef SOCCER_SLIDER_H
#define SOCCER_SLIDER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "soccer/gui/Button.h"

namespace Soccer {

class Slider : public Button {
	public:
		Slider(const char* text, TTF_Font* font, const Common::Rectangle& dim, float value);
		virtual bool clicked(int x, int y) override;
		virtual void draw(int screenWidth, int screenHeight) override;
		float getValue();

	private:
		float mValue;
};

}

#endif

