#include <stdexcept>

#include <GL/gl.h>

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

void Button::draw(int screenWidth, int screenHeight)
{
	const Rectangle& r = this->getRectangle();
	if(!this->isTransparent()) {
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		const Color& c1 = this->getColor1();
		const Color& c2 = this->getColor2();
		glColor3ub(c1.r, c1.g, c1.b);
		glVertex3f(r.x, screenHeight - r.y, 1.0f);
		glVertex3f(r.x + r.w, screenHeight - r.y, 1.0f);
		glColor3ub(c2.r, c2.g, c2.b);
		glVertex3f(r.x + r.w, screenHeight - r.y - r.h, 1.0f);
		glVertex3f(r.x, screenHeight - r.y - r.h, 1.0f);
		glEnd();
	}

	float tw2 = 0.5f * this->getTextTexture()->getWidth() * this->getTextWidth();
	float th2 = 0.5f * this->getTextTexture()->getHeight() * this->getTextHeight();

	const Common::Color& textcolor = this->getTextColor();
	glColor3ub(textcolor.r, textcolor.g, textcolor.b);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->getTextTexture()->getTexture());
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);

	Rectangle textbox(r.x, r.y,
			2.0f * tw2, 2.0f * th2);

	switch(this->centeredText()) {
		case TextAlignment::TopLeft:
			break;

		case TextAlignment::TopMiddle:
			textbox.x += r.w * 0.5f - tw2;
			break;

		case TextAlignment::TopRight:
			textbox.x += r.w - 2.0f * tw2;
			break;

		case TextAlignment::MiddleLeft:
			textbox.y += r.h * 0.5f - th2;
			break;

		case TextAlignment::Centered:
			textbox.x += r.w * 0.5f - tw2;
			textbox.y += r.h * 0.5f - th2;
			break;

		case TextAlignment::MiddleRight:
			textbox.x += r.w - 2.0f * tw2;
			textbox.y += r.h * 0.5f - th2;
			break;

		case TextAlignment::BottomLeft:
			textbox.y += r.h - 2.0f * th2;
			break;

		case TextAlignment::BottomMiddle:
			textbox.x += r.w * 0.5f - tw2;
			textbox.y += r.h - 2.0f * th2;
			break;

		case TextAlignment::BottomRight:
			textbox.x += r.w - 2.0f * tw2;
			textbox.y += r.h - 2.0f * th2;
			break;
	}

	textbox.y = screenHeight - textbox.y;
	glVertex3f(textbox.x,             textbox.y, 1.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(textbox.x + textbox.w, textbox.y, 1.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(textbox.x + textbox.w, textbox.y - textbox.h, 1.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(textbox.x,             textbox.y - textbox.h, 1.1f);

	glEnd();
}

}
