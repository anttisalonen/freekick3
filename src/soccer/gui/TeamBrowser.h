#ifndef SOCCER_TEAMBROWSER_H
#define SOCCER_TEAMBROWSER_H

#include <map>
#include <string>
#include <memory>

#include "soccer/Team.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

enum class TeamSelection {
	None,
	Human,
	Computer
};

class TeamBrowser : public Screen {
	public:
		TeamBrowser(std::shared_ptr<ScreenManager> sm);
		void buttonPressed(std::shared_ptr<Button> button);

	protected:
		virtual bool clickedOnTeam(std::shared_ptr<Button> button);
		virtual bool clickedOnLeague(std::shared_ptr<Button> button);
		virtual bool clickedOnCountry(std::shared_ptr<Button> button);
		virtual bool clickedOnContinent(std::shared_ptr<Button> button);
		virtual bool canClickDone() = 0;
		virtual void clickedDone() = 0;
		std::map<std::shared_ptr<Team>, TeamSelection> mSelectedTeams;

	private:
		void clearCurrentButtons();
		void addContinentButtons();
		void addCountryButtons(std::shared_ptr<Continent> c);
		void addLeagueButtons(std::shared_ptr<LeagueSystem> c);
		void addTeamButtons(std::shared_ptr<League> l);
		void addSelectionButton(const char* text, int i, int maxnum);
		void teamClicked(std::shared_ptr<Button> button);
		std::shared_ptr<Button> mPlayButton;
		std::map<std::string, std::shared_ptr<Continent>> mContinentButtons;
		std::map<std::string, std::shared_ptr<LeagueSystem>> mCountryButtons;
		std::map<std::string, std::shared_ptr<League>> mLeagueButtons;
		std::map<std::string, std::shared_ptr<Team>> mTeamButtons;
		std::vector<std::shared_ptr<Button>> mCurrentButtons;
		std::shared_ptr<Continent> mCurrentContinent;
		std::shared_ptr<LeagueSystem> mCurrentCountry;
		std::shared_ptr<League> mCurrentLeague;
		int mCurrentLevel;
};

}

#endif


