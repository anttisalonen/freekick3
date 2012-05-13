#include "soccer/Match.h"

#include "soccer/gui/FriendlyScreen.h"

namespace Soccer {

FriendlyScreen::FriendlyScreen(std::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addButton("Play", Common::Rectangle(0.35f, 0.35f, 0.30f, 0.15f));
	addButton("Back", Common::Rectangle(0.35f, 0.65f, 0.30f, 0.15f));
}

void FriendlyScreen::buttonPressed(const std::string& buttonText)
{
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
	}
	else if(buttonText == "Play") {
		auto it = mScreenManager->getTeamDatabase().find(1);
		if(it == mScreenManager->getTeamDatabase().end()) {
			std::cerr << "Could not find team with ID 1\n";
			return;
		}
		std::shared_ptr<Team> t1 = it->second;
		it = mScreenManager->getTeamDatabase().find(2);
		if(it == mScreenManager->getTeamDatabase().end()) {
			std::cerr << "Could not find team with ID 2\n";
			return;
		}
		std::shared_ptr<Team> t2 = it->second;
		Match m(t1, t2, TeamTactics(), TeamTactics());
		DataExchange::createMatchDataFile(m, "tmp/match.xml");
		return;
	}
}


}
