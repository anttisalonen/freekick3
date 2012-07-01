#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"
#include "soccer/League.h"

#include "soccer/gui/PresetLeagueScreen.h"
#include "soccer/gui/LeagueScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

PresetLeagueScreen::PresetLeagueScreen(boost::shared_ptr<ScreenManager> sm)
	: TeamBrowser(sm)
{
}

bool PresetLeagueScreen::enteringLeague(boost::shared_ptr<League> p)
{
	mSelectedTeams.clear();
	for(auto t : p->getContainer()) {
		mSelectedTeams.insert(std::make_pair(t.second, TeamSelection::Computer));
	}
	return true;
}

bool PresetLeagueScreen::canClickDone()
{
	return getCurrentLevel() == 3;
}

void PresetLeagueScreen::clickedDone()
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;

	for(auto t : mSelectedTeams) {
		teams.push_back(boost::shared_ptr<StatefulTeam>(new StatefulTeam(*t.first,
						TeamController(t.second == TeamSelection::Human,
							0), AITactics::createTeamTactics(*t.first))));
	}

	boost::shared_ptr<StatefulLeague> league(new StatefulLeague(teams));
	mScreenManager->addScreen(boost::shared_ptr<Screen>(new LeagueScreen(mScreenManager, league)));
}

const std::string PresetLeagueScreen::ScreenName = std::string("Preset League Screen");

const std::string& PresetLeagueScreen::getName() const
{
	return ScreenName;
}

bool PresetLeagueScreen::clickingOnTeam(boost::shared_ptr<Team> p)
{
	auto it2 = mSelectedTeams.find(p);
	if(it2 != mSelectedTeams.end()) {
		if(it2->second == TeamSelection::Computer) {
			it2->second = TeamSelection::Human;
		}
		else {
			it2->second = TeamSelection::Computer;
		}
	}
	return true;
}

}
