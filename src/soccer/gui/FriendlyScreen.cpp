#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/FriendlyScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

FriendlyScreen::FriendlyScreen(std::shared_ptr<ScreenManager> sm)
	: TeamBrowser(sm)
{
}

bool FriendlyScreen::canClickDone()
{
	if(mSelectedTeams.size() == 2) {
		int numHumans = 0;
		for(auto it3 : mSelectedTeams) {
			if(it3.second == TeamSelection::Human)
				numHumans++;
		}
		return numHumans < 2;
	}
	return false;
}

void FriendlyScreen::clickedDone()
{
	int teamnum = 0;

	assert(mSelectedTeams.size() == 2);

	std::vector<std::shared_ptr<Team>> teams;
	int thisteamnum = 1;
	for(auto it : mSelectedTeams) {
		teams.push_back(it.first);
		if(it.second == TeamSelection::Human)
			teamnum = thisteamnum;
		thisteamnum++;
	}
	Match m(std::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[0], TeamController(teamnum == 1, 0), TeamTactics())),
				std::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[1], TeamController(teamnum == 2, 0), TeamTactics())));
	MatchResult res = m.play(true);
	m.setResult(res);
	mScreenManager->addScreen(std::shared_ptr<Screen>(new MatchResultScreen(mScreenManager, m)));

	return;
}

const std::string FriendlyScreen::ScreenName = std::string("Friendly Screen");

const std::string& FriendlyScreen::getName() const
{
	return ScreenName;
}


}
