#include "Screen.h"

using namespace Common;

namespace Soccer {

Screen::Screen(std::shared_ptr<ScreenManager> sm)
	: mScreenManager(sm)
{
}

std::shared_ptr<Button> Screen::addButton(const char* text, const Rectangle& dim)
{
	std::shared_ptr<Button> b(new Button(text, mScreenManager->getFont(),
					Rectangle(dim.x * mScreenManager->getScreenWidth(),
						dim.y * mScreenManager->getScreenHeight(),
						dim.w * mScreenManager->getScreenWidth(),
						dim.h * mScreenManager->getScreenHeight())));
	setButtonTextSize(b);
	mButtons.push_back(b);
	return b;
}

bool Screen::removeButton(std::shared_ptr<Button> b)
{
	for(auto it = mButtons.begin(); it != mButtons.end(); ++it) {
		if(*it == b) {
			mButtons.erase(it);
			return true;
		}
	}
	return false;
}

std::shared_ptr<Button> Screen::addLabel(const char* text, float x, float y, TextAlignment centered, float fsize)
{
	float xp = x;
	float yp = y;
	const float w2 = 0.25f, h2 = 0.05f;

	switch(centered) {
		case TextAlignment::TopLeft:
			break;

		case TextAlignment::TopMiddle:
			xp -= w2;
			break;

		case TextAlignment::TopRight:
			xp -= 2.0f * w2;
			break;

		case TextAlignment::MiddleLeft:
			yp -= h2;
			break;

		case TextAlignment::Centered:
			xp -= w2;
			yp -= h2;
			break;

		case TextAlignment::MiddleRight:
			xp -= 2.0f * w2;
			yp -= h2;
			break;

		case TextAlignment::BottomLeft:
			yp -= 2.0f * h2;
			break;

		case TextAlignment::BottomMiddle:
			xp -= w2;
			yp -= 2.0f * h2;
			break;

		case TextAlignment::BottomRight:
			xp -= 2.0f * w2;
			yp -= 2.0f * h2;
			break;
	}
	std::shared_ptr<Button> b = addButton(text, Rectangle(xp,
				yp, 2.0f * w2, 2.0f * h2));
	b->setTransparent(true);
	b->deactivate();
	b->setCenteredText(centered);
	b->setTextWidth(fsize);
	b->setTextHeight(fsize);
	return b;
}

const std::vector<std::shared_ptr<Button>>& Screen::getButtons() const
{
	return mButtons;
}

void Screen::setButtonTextSize(std::shared_ptr<Button> b)
{
	const Common::Rectangle& dim = b->getRectangle();
	float texwidth_pix = b->getTextTexture()->getWidth();
	float texheight_pix = b->getTextTexture()->getHeight();
	float tw = 1.0f;
	while(tw > 0.3f && (texwidth_pix * tw > dim.w || texheight_pix * tw > dim.h)) {
		tw -= 0.2f;
	}
	b->setTextWidth(tw);
	b->setTextHeight(tw);
}

}

