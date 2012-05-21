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

bool PresetLeagueScreen::clickedOnLeague(std::shared_ptr<Button> button)
{
	return true;
}

bool PresetLeagueScreen::canClickDone()
{
	return false;
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
