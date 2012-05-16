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

std::shared_ptr<Button> Screen::addLabel(const char* text, float x, float y, bool centered)
{
	std::shared_ptr<Button> b(new Button(text, mScreenManager->getFont(),
				Rectangle(x * mScreenManager->getScreenWidth() - (centered ? 100 : 0),
					y * mScreenManager->getScreenHeight(),
					centered ? 200 : (0.2f * mScreenManager->getScreenWidth()),
					48)));
	b->setTransparent(true);
	b->deactivate();
	b->setCenteredText(centered);
	mButtons.push_back(b);
	return b;
}

const std::vector<std::shared_ptr<Button>>& Screen::getButtons() const
{
	return mButtons;
}

}

