#include "common/Rectangle.h"

#include "soccer/Tournament.h"
#include "soccer/Team.h"
#include "soccer/Match.h"

#include "soccer/gui/PresetTournamentScreen.h"
#include "soccer/gui/TournamentScreen.h"
#include "soccer/gui/TeamSelectionScreen.h"

using namespace Common;

namespace Soccer {

PresetTournamentScreen::PresetTournamentScreen(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	float x = 0.35f;
	float y = 0.35f;

	/* TODO: move selection button coordinate generation from TeamBrowser to
	 * someplace generic and use that. */
	for(auto& p : mScreenManager->getTeamDatabase().getContainer()) {
		for(auto& t : p.second->getTournaments()) {
			auto b = addButton(t.getName().c_str(), Rectangle(x, y, 0.30f, 0.07f));
			mTournamentButtons.insert({b, t});
			y += 0.10f;
			if(y > 0.80f) {
				goto done;
			}
		}
	}
done:

	addButton("Back",                  Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
}

void PresetTournamentScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
	} else {
		for(auto& p : mTournamentButtons) {
			if(button == p.first) {
				std::vector<boost::shared_ptr<Team>> teams;
				teams = getTournamentTeams(p.second);
				mScreenManager->addScreen(boost::shared_ptr<Screen>(new
							TeamSelectionScreen(mScreenManager, teams,
								std::move(std::unique_ptr<TournamentConfig>(new TournamentConfig(p.second))))));
				return;
			}
		}
	}
}

const std::string PresetTournamentScreen::ScreenName = "Preset Tournament Screen";

const std::string& PresetTournamentScreen::getName() const
{
	return ScreenName;
}

std::vector<boost::shared_ptr<Team>> PresetTournamentScreen::getTournamentTeams(const TournamentConfig& tc)
{
	std::vector<boost::shared_ptr<Team>> ret;
	auto tdb = mScreenManager->getTeamDatabase();
	for(auto& ts : tc.getTournamentStages()) {
		for(auto& tsd : ts->getTeamSources()) {
			auto continent = tdb.getOrCreateContinent(tsd.Continent.c_str());
			boost::shared_ptr<LeagueSystem> leaguesystem;
			boost::shared_ptr<League> league;

			if(!tsd.LeagueSystem.empty()) {
				auto it = continent->getContainer().find(tsd.LeagueSystem);
				if(it != continent->getContainer().end())
					leaguesystem = it->second;
			} else if(!continent->getContainer().empty()) {
				// just pick the alphabetically first one
				leaguesystem = continent->getContainer().begin()->second;
			}
			if(!leaguesystem) {
				std::cerr << "League system '" << tsd.LeagueSystem << "' not found - skipping.\n";
				continue;
			}

			if(!tsd.League.empty()) {
				auto it = leaguesystem->getContainer().find(tsd.League);
				if(it != leaguesystem->getContainer().end())
					league = it->second;
			} else {
				for(auto& l : leaguesystem->getContainer()) {
					if(l.second->getLevel() == 0) {
						league = l.second;
						break;
					}
				}
			}

			if(!league) {
				std::cerr << "League '" << tsd.League << "' not found in league system '"
					<< tsd.LeagueSystem << "' - skipping.\n";
				continue;
			}

			// sort teams by position
			std::map<unsigned int, boost::shared_ptr<Team>> teams;
			for(auto& team : league->getContainer()) {
				teams.insert({team.second->getPosition(), team.second});
			}

			unsigned int s(tsd.Skip);
			unsigned int t(tsd.Teams);

			// tsd.Skip starts at 1
			if(s)
				s--;

			for(auto& team : teams) {
				if(s) {
					s--;
					continue;
				}

				if(!t) {
					break;
				}
				ret.push_back(team.second);
				t--;
			}
		}
	}

	return ret;
}

}

