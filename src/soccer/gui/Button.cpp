#include <stdexcept>

#include "common/Texture.h"

#include "soccer/gui/Button.h"

namespace Soccer {

using namespace Common;

Color Button::DefaultColor1 = Color(217, 191, 128);
Color Button::DefaultColor2 = Color(191, 128, 115);

Button::Button(const char* text, TTF_Font* font, const Rectangle& dim)
	: Widget(dim),
	mText(std::string(text)),
	mFont(font),
	mTransparent(false),
	mCenteredText(TextAlignment::Centered),
	mColor1(DefaultColor1),
	mColor2(DefaultColor2),
	mTextColor(Common::Color::White),
	mTextWidth(0.5f),
	mTextHeight(0.5f)
{
	setTexture();
}

void Button::setTexture()
{
	SDL_Surface* textsurface;
	SDL_Color color = {255, 255, 255};
	textsurface = TTF_RenderUTF8_Blended(mFont, mText.c_str(), color);

	if(!textsurface) {
		fprintf(stderr, "Could not render text: %s\n",
				TTF_GetError());
		throw std::runtime_error("Rendering text");
	}
	else {
		mTextTexture = boost::shared_ptr<Texture>(new Texture(textsurface));
		SDL_FreeSurface(textsurface);
	}
}

const std::string& Button::getText() const
{
	return mText;
}

void Button::setText(const std::string& t)
{
	if(t != mText) {
		mText = t;
		setTexture();
	}
}

const Common::Texture* Button::getTextTexture() const
{
	return mTextTexture.get();
}

bool Button::isTransparent() const
{
	return mTransparent;
}

void Button::setTransparent(bool t)
{
	mTransparent = t;
}

TextAlignment Button::centeredText() const
{
	return mCenteredText;
}

void Button::setCenteredText(TextAlignment c)
{
	mCenteredText = c;
}

const Color& Button::getColor1() const
{
	return mColor1;
}

const Color& Button::getColor2() const
{
	return mColor2;
}

void Button::setColor1(const Common::Color& c)
{
	mColor1 = c;
}

void Button::setColor2(const Common::Color& c)
{
	mColor2 = c;
}

void Button::setTextWidth(float f)
{
	mTextWidth = f;
}

void Button::setTextHeight(float f)
{
	mTextHeight = f;
}

float Button::getTextWidth() const
{
	return mTextWidth;
}

float Button::getTextHeight() const
{
	return mTextHeight;
}

void Button::setTextColor(const Common::Color& c)
{
	mTextColor = c;
}

const Common::Color& Button::getTextColor() const
{
	return mTextColor;
}

}
