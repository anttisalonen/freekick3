#include "soccer/Match.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

MatchResultScreen::MatchResultScreen(boost::shared_ptr<ScreenManager> sm, const Match& match)
	: Screen(sm)
{
	addButton("To Menu", Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	addButton("Replay",  Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));

	addLabel(match.getTeam(0)->getName().c_str(), 0.25f, 0.1f);
	addLabel(match.getTeam(1)->getName().c_str(), 0.75f, 0.1f);
	const MatchResult& mres = match.getResult();
	if(mres.Played) {
		addLabel("Goals", 0.5f, 0.2f);
		addLabel(std::to_string(mres.HomeGoals).c_str(), 0.25f, 0.2f);
		addLabel(std::to_string(mres.AwayGoals).c_str(), 0.75f, 0.2f);
	}
	else {
		addLabel("No result", 0.5f, 0.2f);
	}
}

void MatchResultScreen::buttonPressed(boost::shared_ptr<Button> button)
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

