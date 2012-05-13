#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

MatchResultScreen::MatchResultScreen(std::shared_ptr<ScreenManager> sm, const char* resultfilename)
	: Screen(sm)
{
	addButton("Replay",  Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	addButton("To Menu", Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));
}

void MatchResultScreen::buttonPressed(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Replay") {
		mScreenManager->dropScreensUntil("Friendly Screen");
	}
	else if(buttonText == "To Menu") {
		mScreenManager->dropScreensUntil("Main Menu");
	}
}

const std::string MatchResultScreen::ScreenName = std::string("Match Result Screen");

const std::string& MatchResultScreen::getName() const
{
	return ScreenName;
}


}

