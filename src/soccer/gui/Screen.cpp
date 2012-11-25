#include "Screen.h"

using namespace Common;

namespace Soccer {

Screen::Screen(boost::shared_ptr<ScreenManager> sm)
	: mScreenManager(sm)
{
}

boost::shared_ptr<Button> Screen::addButton(const char* text, const Rectangle& dim,
		bool keyShortcut, SDLKey shortcutKey, int modifier)
{
	boost::shared_ptr<Button> b(new Button(text, mScreenManager->getFont(),
					Rectangle(dim.x * mScreenManager->getScreenWidth(),
						dim.y * mScreenManager->getScreenHeight(),
						dim.w * mScreenManager->getScreenWidth(),
						dim.h * mScreenManager->getScreenHeight())));
	setButtonTextSize(b);
	mButtons.push_back(b);
	if(keyShortcut) {
		auto& v = mKeyShortcuts[shortcutKey];
		v.push_back({modifier, b});
	}
	return b;
}

boost::shared_ptr<Button> Screen::getKeyboardShortcuts(SDLKey k, SDLMod m) const
{
	auto it = mKeyShortcuts.find(k);
	if(it != mKeyShortcuts.end()) {
		for(auto& p : it->second) {
			if((p.first == m) || (p.first & m))
				return p.second;
		}
	}

	return boost::shared_ptr<Button>();
}

bool Screen::removeButton(boost::shared_ptr<Button> b)
{
	for(auto it = mButtons.begin(); it != mButtons.end(); ++it) {
		if(*it == b) {
			mButtons.erase(it);
			return true;
		}
	}
	return false;
}

bool Screen::removeSlider(boost::shared_ptr<Slider> b)
{
	for(auto it = mSliders.begin(); it != mSliders.end(); ++it) {
		if(*it == b) {
			mSliders.erase(it);
			return true;
		}
	}
	return false;
}

boost::shared_ptr<Slider> Screen::addSlider(const char* text, const Common::Rectangle& dim,
		float value)
{
	boost::shared_ptr<Slider> b(new Slider(text, mScreenManager->getFont(),
					Rectangle(dim.x * mScreenManager->getScreenWidth(),
						dim.y * mScreenManager->getScreenHeight(),
						dim.w * mScreenManager->getScreenWidth(),
						dim.h * mScreenManager->getScreenHeight()),
					value));
	setButtonTextSize(b);
	mSliders.push_back(b);
	return b;
}

boost::shared_ptr<Button> Screen::addLabel(const char* text, float x, float y, TextAlignment centered,
		float fsize, Common::Color col)
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
	boost::shared_ptr<Button> b = addButton(text, Rectangle(xp,
				yp, 2.0f * w2, 2.0f * h2));
	b->setTextColor(col);
	b->setTransparent(true);
	b->deactivate();
	b->setCenteredText(centered);
	b->setTextWidth(fsize);
	b->setTextHeight(fsize);
	return b;
}

std::vector<boost::shared_ptr<Button>>& Screen::getButtons()
{
	return mButtons;
}

std::vector<boost::shared_ptr<Slider>>& Screen::getSliders()
{
	return mSliders;
}

void Screen::setButtonTextSize(boost::shared_ptr<Button> b)
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

boost::shared_ptr<Image> Screen::addImage(const char* filepath, const Common::Rectangle& dim)
{
	boost::shared_ptr<Image> i(new Image(filepath,
					Rectangle(dim.x * mScreenManager->getScreenWidth(),
						dim.y * mScreenManager->getScreenHeight(),
						dim.w * mScreenManager->getScreenWidth(),
						dim.h * mScreenManager->getScreenHeight())));
	mImages.push_back(i);
	return i;
}

bool Screen::removeImage(boost::shared_ptr<Image> i)
{
	for(auto it = mImages.begin(); it != mImages.end(); ++it) {
		if(*it == i) {
			mImages.erase(it);
			return true;
		}
	}
	return false;
}

std::vector<boost::shared_ptr<Image>>& Screen::getImages()
{
	return mImages;
}

}

