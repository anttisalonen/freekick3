#include "soccer/gui/MainMenuScreen.h"
#include "soccer/gui/FriendlyScreen.h"
#include "soccer/gui/PresetLeagueScreen.h"
#include "soccer/gui/PresetCupScreen.h"
#include "soccer/gui/PresetSeasonScreen.h"
#include "soccer/gui/PresetTournamentScreen.h"
#include "soccer/gui/LoadGameScreen.h"
#include "soccer/gui/UsageScreen.h"

namespace Soccer {

using namespace Common;

MainMenuScreen::MainMenuScreen(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addLabel("Freekick 3", 0.5f, 0.25f, TextAlignment::Centered, 3.0f, Color(255, 255, 255));
	addButton("Friendly",          Rectangle(0.15f, 0.45f, 0.30f, 0.07f));
	addButton("Preset League",     Rectangle(0.55f, 0.45f, 0.30f, 0.07f));
	addButton("Preset Cup",        Rectangle(0.15f, 0.55f, 0.30f, 0.07f));
	addButton("Preset Season",     Rectangle(0.55f, 0.55f, 0.30f, 0.07f));
	addButton("Preset Tournament", Rectangle(0.15f, 0.65f, 0.30f, 0.07f));
	//addButton("Career",            Rectangle(0.55f, 0.65f, 0.30f, 0.07f));
	addButton("Load Game",         Rectangle(0.15f, 0.75f, 0.30f, 0.07f));
	addButton("Usage",             Rectangle(0.55f, 0.75f, 0.30f, 0.07f));
	addButton("Quit",              Rectangle(0.35f, 0.85f, 0.30f, 0.07f));
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
	else if(buttonText == "Preset Cup") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new PresetCupScreen(mScreenManager)));
	}
	else if(buttonText == "Preset Season") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new PresetSeasonScreen(mScreenManager)));
	}
	else if(buttonText == "Preset Tournament") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new PresetTournamentScreen(mScreenManager)));
	}
	else if(buttonText == "Load Game") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new LoadGameScreen(mScreenManager)));
	}
	else if(buttonText == "Usage") {
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new UsageScreen(mScreenManager)));
	}
}

const std::string MainMenuScreen::ScreenName = std::string("Main Menu");

const std::string& MainMenuScreen::getName() const
{
	return ScreenName;
}

}
