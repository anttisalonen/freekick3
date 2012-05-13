#include "soccer/Match.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

MatchResultScreen::MatchResultScreen(std::shared_ptr<ScreenManager> sm, const char* resultfilename)
	: Screen(sm)
{
	addButton("To Menu", Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	addButton("Replay",  Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));

	std::shared_ptr<Match> match = DataExchange::parseMatchDataFile(resultfilename);
	addLabel(match->getTeam(0)->getName().c_str(), 0.25f, 0.1f, true);
	addLabel(match->getTeam(1)->getName().c_str(), 0.75f, 0.1f, true);
	const MatchResult& mres = match->getResult();
	if(mres.Played) {
		addLabel("Goals", 0.5f, 0.2f, true);
		addLabel(std::to_string(mres.HomeGoals).c_str(), 0.25f, 0.2f, true);
		addLabel(std::to_string(mres.AwayGoals).c_str(), 0.75f, 0.2f, true);
	}
	else {
		addLabel("No result", 0.5f, 0.2f, true);
	}
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

