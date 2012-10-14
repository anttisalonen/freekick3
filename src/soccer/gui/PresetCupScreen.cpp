#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"
#include "soccer/Cup.h"

#include "soccer/gui/PresetCupScreen.h"
#include "soccer/gui/CupScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

PresetCupScreen::PresetCupScreen(boost::shared_ptr<ScreenManager> sm)
	: TeamBrowser(sm)
{
}

bool PresetCupScreen::enteringCountry(boost::shared_ptr<LeagueSystem> p)
{
	auto teams = StatefulCup::collectTeamsFromCountry(p);

	mSelectedTeams.clear();
	for(auto t : teams) {
		mSelectedTeams.insert(std::make_pair(t, TeamSelection::Computer));
	}

	addTeamButtons(teams);

	// return false because we already added the team buttons
	return false;
}

bool PresetCupScreen::canClickDone()
{
	return getCurrentLevel() == TeamBrowserLevel::Teams;
}

void PresetCupScreen::clickedDone()
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;

	teams = createStatefulTeams();

	boost::shared_ptr<StatefulCup> cup(new StatefulCup(teams));
	mScreenManager->addScreen(boost::shared_ptr<Screen>(new CupScreen(mScreenManager, cup)));
}

const std::string PresetCupScreen::ScreenName = std::string("Preset Cup Screen");

const std::string& PresetCupScreen::getName() const
{
	return ScreenName;
}

bool PresetCupScreen::clickingOnTeam(boost::shared_ptr<Team> p)
{
	toggleTeamOwnership(p);
	return true;
}

}
