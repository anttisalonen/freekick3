#include "soccer/Match.h"
#include "soccer/League.h"
#include "soccer/Cup.h"
#include "soccer/Season.h"

#include "soccer/gui/PresetSeasonScreen.h"
#include "soccer/gui/SeasonScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

PresetSeasonScreen::PresetSeasonScreen(boost::shared_ptr<ScreenManager> sm)
	: PresetLeagueScreen(sm)
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
	std::map<boost::shared_ptr<Team>, boost::shared_ptr<StatefulTeam>> allteams;

	boost::shared_ptr<StatefulLeague> league;
	boost::shared_ptr<StatefulCup> cup;

	{
		std::vector<boost::shared_ptr<StatefulTeam>> leagueteams;
		for(auto t : mSelectedTeams) {
			boost::shared_ptr<StatefulTeam> st(new StatefulTeam(*t.first,
						TeamController(t.first == mOwnTeam, 0),
						AITactics::createTeamTactics(*t.first)));
			allteams.insert({t.first, st});
			leagueteams.push_back(st);
		}
		boost::shared_ptr<StatefulLeague> league(new StatefulLeague(leagueteams));
	}

	{
		std::vector<boost::shared_ptr<StatefulTeam>> cupteams;
		std::vector<boost::shared_ptr<Team>> cupteamentries;
		cupteamentries = StatefulCup::collectTeamsFromCountry(mCountry);
		for(auto t : cupteamentries) {
			auto it = allteams.find(t);
			if(it == allteams.end()) {
				cupteams.push_back(boost::shared_ptr<StatefulTeam>(new StatefulTeam(*t,
								TeamController(t == mOwnTeam, 0),
								AITactics::createTeamTactics(*t))));
			} else {
				cupteams.push_back(it->second);
			}
		}
		boost::shared_ptr<StatefulCup> cup(new StatefulCup(cupteams));
	}

	boost::shared_ptr<Season> season(new Season(mOwnTeam, league, cup));
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
