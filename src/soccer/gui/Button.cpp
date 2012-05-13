#include <stdexcept>

#include "common/Texture.h"

#include "soccer/gui/Button.h"

namespace Soccer {

using namespace Common;

Button::Button(const char* text, TTF_Font* font, const Rectangle& dim)
	: mText(std::string(text)),
	mRectangle(dim)
{
	SDL_Surface* textsurface;
	SDL_Color color = {255, 255, 255};
	textsurface = TTF_RenderUTF8_Blended(font, text, color);

	if(!textsurface) {
		fprintf(stderr, "Could not render text: %s\n",
				TTF_GetError());
		throw std::runtime_error("Rendering text");
	}
	else {
		mTextTexture = std::shared_ptr<Texture>(new Texture(textsurface));
		SDL_FreeSurface(textsurface);
	}
}

bool Button::clicked(int x, int y) const
{
	return mRectangle.pointWithin(x, y);
}

const std::string& Button::getText() const
{
	return mText;
}

const Rectangle& Button::getRectangle() const
{
	return mRectangle;
}

const Common::Texture* Button::getTexture() const
{
	return mTextTexture.get();
}

}
