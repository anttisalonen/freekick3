#include "soccer/gui/MainMenuScreen.h"
#include "soccer/gui/FriendlyScreen.h"

namespace Soccer {

using namespace Common;

MainMenuScreen::MainMenuScreen(std::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addButton("Friendly", Rectangle(0.35f, 0.35f, 0.30f, 0.15f));
	addButton("Quit",     Rectangle(0.35f, 0.65f, 0.30f, 0.15f));
}

void MainMenuScreen::buttonPressed(const std::string& buttonText)
{
	if(buttonText == "Quit" || buttonText == "Back") {
		mScreenManager->clearScreens();
	}
	else if(buttonText == "Friendly") {
		mScreenManager->addScreen(std::shared_ptr<Screen>(new FriendlyScreen(mScreenManager)));
	}
}

}
