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

static unsigned int pow2roundup(unsigned int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

bool PresetCupScreen::enteringCountry(boost::shared_ptr<LeagueSystem> p)
{
	mSelectedTeams.clear();
	unsigned int numTeams = 0;
	for(auto league : p->getContainer()) {
		numTeams += league.second->getContainer().size();
	}

	numTeams = pow2roundup(numTeams) / 2;

	for(auto league : p->getContainer()) {
		if(numTeams == 0)
			break;
		for(auto t : league.second->getContainer()) {
			if(numTeams == 0)
				break;
			mSelectedTeams.insert(std::make_pair(t.second, TeamSelection::Computer));
			numTeams--;
		}
	}

	{
		std::vector<boost::shared_ptr<Team>> teams;
		for(auto t : mSelectedTeams) {
			teams.push_back(t.first);
		}
		addTeamButtons(teams);
	}

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
