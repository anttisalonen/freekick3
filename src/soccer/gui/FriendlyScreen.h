#ifndef SOCCER_FRIENDLYSCREEN_H
#define SOCCER_FRIENDLYSCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/gui/Screen.h"
#include "soccer/Team.h"

namespace Soccer {

enum class TeamSelection {
	None,
	Human,
	Computer
};

class FriendlyScreen : public Screen {
	public:
		FriendlyScreen(std::shared_ptr<ScreenManager> sm);
		void buttonPressed(std::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		void playMatch(const char* datafile, int teamnum, int playernum);
		void clearCurrentButtons();
		void addContinentButtons();
		void addCountryButtons(std::shared_ptr<Continent> c);
		void addLeagueButtons(std::shared_ptr<LeagueSystem> c);
		void addTeamButtons(std::shared_ptr<League> l);
		void addSelectionButton(const char* text, int i, int maxnum);
		void clickedOnTeam(std::shared_ptr<Button> button);
		std::shared_ptr<Button> mPlayButton;
		std::map<std::string, std::shared_ptr<Continent>> mContinentButtons;
		std::map<std::string, std::shared_ptr<LeagueSystem>> mCountryButtons;
		std::map<std::string, std::shared_ptr<League>> mLeagueButtons;
		std::map<std::string, std::shared_ptr<Team>> mTeamButtons;
		std::map<std::shared_ptr<Team>, TeamSelection> mSelectedTeams;
		std::vector<std::shared_ptr<Button>> mCurrentButtons;
		static const std::string ScreenName;
		std::shared_ptr<Continent> mCurrentContinent;
		std::shared_ptr<LeagueSystem> mCurrentCountry;
		std::shared_ptr<League> mCurrentLeague;
		int mCurrentLevel;
};

}

#endif


