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
	return getCurrentLevel() == TeamBrowserLevel::Teams;
}

void PresetLeagueScreen::clickedDone()
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;

	teams = createStatefulTeams();

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
	toggleTeamOwnership(p);
	return true;
}

}
