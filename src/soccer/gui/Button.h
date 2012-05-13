#ifndef SOCCER_BUTTON_H
#define SOCCER_BUTTON_H

#include <string>
#include <memory>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/Texture.h"
#include "common/Rectangle.h"

namespace Soccer {

class Button {
	public:
		Button(const char* text, TTF_Font* font, const Common::Rectangle& dim);
		bool clicked(int x, int y) const;
		const std::string& getText() const;
		const Common::Rectangle& getRectangle() const;
		const Common::Texture* getTexture() const;
	private:
		std::string mText;
		Common::Rectangle mRectangle;
		std::shared_ptr<Common::Texture> mTextTexture;
};

}

#endif

