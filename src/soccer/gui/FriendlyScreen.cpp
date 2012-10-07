#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/FriendlyScreen.h"
#include "soccer/gui/MatchResultScreen.h"
#include "soccer/gui/TeamTacticsScreen.h"

namespace Soccer {

FriendlyScreen::FriendlyScreen(boost::shared_ptr<ScreenManager> sm)
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

	std::vector<boost::shared_ptr<Team>> teams;
	int thisteamnum = 1;
	for(auto it : mSelectedTeams) {
		teams.push_back(it.first);
		if(it.second == TeamSelection::Human)
			teamnum = thisteamnum;
		thisteamnum++;
	}

	mMatch = boost::shared_ptr<Match>(new Match(boost::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[0], TeamController(teamnum == 1, 0),
					AITactics::createTeamTactics(*teams[0]))),
			boost::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[1], TeamController(teamnum == 2, 0),
					AITactics::createTeamTactics(*teams[1]))),
			MatchRules(false, false)));

	mScreenManager->addScreen(boost::shared_ptr<Screen>(new TeamTacticsScreen(mScreenManager, *mMatch,
					[&](Match& m) -> void {
					RunningMatch rm = RunningMatch(m);
					MatchResult r;
					while(!rm.matchFinished(&r)) {
						sleep(1);
						mScreenManager->drawScreen();
					}
					m.setResult(r);
					mScreenManager->dropScreen();
					mScreenManager->addScreen(boost::shared_ptr<Screen>(new MatchResultScreen(mScreenManager, m)));
					})));

	return;
}

const std::string FriendlyScreen::ScreenName = std::string("Friendly Screen");

const std::string& FriendlyScreen::getName() const
{
	return ScreenName;
}


}
