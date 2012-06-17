#include "soccer/gui/MainMenuScreen.h"
#include "soccer/gui/FriendlyScreen.h"
#include "soccer/gui/PresetLeagueScreen.h"
#include "soccer/gui/LoadGameScreen.h"

namespace Soccer {

using namespace Common;

MainMenuScreen::MainMenuScreen(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addButton("Friendly",      Rectangle(0.35f, 0.40f, 0.30f, 0.10f));
	addButton("Preset League", Rectangle(0.35f, 0.55f, 0.30f, 0.10f));
	addButton("Load Game",     Rectangle(0.35f, 0.70f, 0.30f, 0.10f));
	addButton("Quit",          Rectangle(0.35f, 0.85f, 0.30f, 0.10f));
}

void MainMenuScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Quit" || buttonText == "Back") {
		mScreenManager->clearScreens();
	}
	else if(buttonText == "Friendly") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new FriendlyScreen(mScreenManager)));
	}
	else if(buttonText == "Preset League") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new PresetLeagueScreen(mScreenManager)));
	}
	else if(buttonText == "Load Game") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new LoadGameScreen(mScreenManager)));
	}
}

const std::string MainMenuScreen::ScreenName = std::string("Main Menu");

const std::string& MainMenuScreen::getName() const
{
	return ScreenName;
}

}
