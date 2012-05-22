#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/PresetLeagueScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

PresetLeagueScreen::PresetLeagueScreen(std::shared_ptr<ScreenManager> sm)
	: TeamBrowser(sm)
{
}

bool PresetLeagueScreen::enteringLeague(std::shared_ptr<League> p)
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
}

const std::string PresetLeagueScreen::ScreenName = std::string("Preset League Screen");

const std::string& PresetLeagueScreen::getName() const
{
	return ScreenName;
}


}
