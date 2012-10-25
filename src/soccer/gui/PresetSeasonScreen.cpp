#include "soccer/Match.h"
#include "soccer/League.h"
#include "soccer/Cup.h"
#include "soccer/Season.h"

#include "soccer/gui/PresetSeasonScreen.h"
#include "soccer/gui/SeasonScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

PresetSeasonScreen::PresetSeasonScreen(boost::shared_ptr<ScreenManager> sm, bool career)
	: PresetLeagueScreen(sm),
	mCareer(career)
{
}

bool PresetSeasonScreen::enteringCountry(boost::shared_ptr<LeagueSystem> p)
{
	mCountry = p;
	return true;
}

bool PresetSeasonScreen::enteringLeague(boost::shared_ptr<League> p)
{
	if(!PresetLeagueScreen::enteringLeague(p))
		return false;

	mLeague = p;
	mOwnTeam = boost::shared_ptr<Team>();

	return true;
}

void PresetSeasonScreen::clickedDone()
{
	boost::shared_ptr<Season> season = Season::createSeason(mOwnTeam, mCountry, mCareer);
	mScreenManager->addScreen(boost::shared_ptr<Screen>(new SeasonScreen(mScreenManager, season)));
}

bool PresetSeasonScreen::canClickDone()
{
	return PresetLeagueScreen::canClickDone() && mOwnTeam;
}

const std::string PresetSeasonScreen::ScreenName = std::string("Preset Season Screen");

const std::string& PresetSeasonScreen::getName() const
{
	return ScreenName;
}

bool PresetSeasonScreen::clickingOnTeam(boost::shared_ptr<Team> p)
{
	if(mOwnTeam && p == mOwnTeam) {
		toggleTeamOwnership(p);
		mOwnTeam = boost::shared_ptr<Team>();
	} else if(!mOwnTeam) {
		toggleTeamOwnership(p);
		mOwnTeam = p;
	}

	return true;
}

}
