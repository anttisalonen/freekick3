#ifndef SOCCER_TEAMBROWSER_H
#define SOCCER_TEAMBROWSER_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Team.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

enum class TeamSelection {
	None,
	Human,
	Computer
};

enum class TeamBrowserLevel {
	Continents,
	Countries,
	Leagues,
	Teams
};

class TeamBrowser : public Screen {
	public:
		TeamBrowser(boost::shared_ptr<ScreenManager> sm);
		void buttonPressed(boost::shared_ptr<Button> button);

	protected:
		virtual bool enteringContinent(boost::shared_ptr<Continent> p);
		virtual bool enteringCountry(boost::shared_ptr<LeagueSystem> p);
		virtual bool enteringLeague(boost::shared_ptr<League> p);
		virtual bool clickingOnTeam(boost::shared_ptr<Team> p);
		virtual bool canClickDone() = 0;
		virtual void clickedDone() = 0;
		std::map<boost::shared_ptr<Team>, TeamSelection> mSelectedTeams;
		TeamBrowserLevel getCurrentLevel() const;
		void setCurrentLevel(TeamBrowserLevel t);
		void toggleTeamOwnership(boost::shared_ptr<Team> p);
		std::vector<boost::shared_ptr<StatefulTeam>> createStatefulTeams() const;
		void addTeamButtons(const std::vector<boost::shared_ptr<Team>>& teams);

	private:
		void clearCurrentButtons();
		void addContinentButtons();
		void addCountryButtons(boost::shared_ptr<Continent> c);
		void addLeagueButtons(boost::shared_ptr<LeagueSystem> c);
		void addTeamButtons(boost::shared_ptr<League> l);
		void addSelectionButton(const char* text, int i, int maxnum);
		void teamClicked(boost::shared_ptr<Button> button);
		void setTeamButtonColor(boost::shared_ptr<Button> button) const;

		boost::shared_ptr<Button> mPlayButton;
		std::map<std::string, boost::shared_ptr<Continent>> mContinentButtons;
		std::map<std::string, boost::shared_ptr<LeagueSystem>> mCountryButtons;
		std::map<std::string, boost::shared_ptr<League>> mLeagueButtons;
		std::map<std::string, boost::shared_ptr<Team>> mTeamButtons;
		std::vector<boost::shared_ptr<Button>> mCurrentButtons;
		boost::shared_ptr<Continent> mCurrentContinent;
		boost::shared_ptr<LeagueSystem> mCurrentCountry;
		boost::shared_ptr<League> mCurrentLeague;
		TeamBrowserLevel mCurrentLevel;
};

}

#endif


