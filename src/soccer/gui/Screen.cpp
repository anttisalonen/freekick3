#include "Screen.h"

using namespace Common;

namespace Soccer {

Screen::Screen(std::shared_ptr<ScreenManager> sm)
	: mScreenManager(sm)
{
}

void Screen::addButton(const char* text, const Rectangle& dim)
{
	mButtons.push_back(std::shared_ptr<Button>(new Button(text, mScreenManager->getFont(),
					Rectangle(dim.x * mScreenManager->getScreenWidth(),
						dim.y * mScreenManager->getScreenHeight(),
						dim.w * mScreenManager->getScreenWidth(),
						dim.h * mScreenManager->getScreenHeight()))));
}

const std::vector<std::shared_ptr<Button>>& Screen::getButtons() const
{
	return mButtons;
}

}

