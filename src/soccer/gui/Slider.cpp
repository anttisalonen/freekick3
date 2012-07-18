#include <stdexcept>

#include "common/Texture.h"
#include "common/Math.h"

#include "soccer/gui/Slider.h"

namespace Soccer {

using namespace Common;

Slider::Slider(const char* text, TTF_Font* font, const Rectangle& dim, float value)
	: Button(text, font, dim),
	mValue(value)
{
	mValue = Common::clamp(0.0f, mValue, 1.0f);
}

bool Slider::clicked(int x, int y)
{
	if(Widget::clicked(x, y)) {
		mValue = (x - mRectangle.x) / (float)mRectangle.w;
		assert(mValue >= 0.0f && mValue <= 1.0f);
		return true;
	}
	return false;
}

void Slider::draw(int screenWidth, int screenHeight)
{
	Button::draw(screenWidth, screenHeight);
	const Rectangle& r = this->getRectangle();
	const float lineWidth = 1.0f;
	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex3f(r.x + r.w * mValue - lineWidth, screenHeight - r.y, 0.5f);
	glVertex3f(r.x + r.w * mValue + lineWidth, screenHeight - r.y, 0.5f);
	glVertex3f(r.x + r.w * mValue + lineWidth, screenHeight - r.y - r.h, 0.5f);
	glVertex3f(r.x + r.w * mValue - lineWidth, screenHeight - r.y - r.h, 0.5f);
	glEnd();
}

float Slider::getValue()
{
	return mValue;
}

}
