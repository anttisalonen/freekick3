#ifndef SOCCER_TEAMTACTICSSCREEN_H
#define SOCCER_TEAMTACTICSSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Match.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

class TeamTacticsScreenOwner {
	public:
		~TeamTacticsScreenOwner() { }
		virtual void TeamTacticsScreenFinished(int playernum) = 0;
};

class TeamTacticsScreen : public Screen {
	public:
		TeamTacticsScreen(boost::shared_ptr<ScreenManager> sm, Match& m,
				TeamTacticsScreenOwner& ttso);
		void buttonPressed(boost::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		void setupPlrLabels();
		void addFormationButton(int i, int formbutton, const std::string& str);
		static const std::string ScreenName;
		Match& mMatch;
		std::vector<boost::shared_ptr<Button>> mPlayerButtons;
		TeamTacticsScreenOwner& mTtso;
		int mChosenplnum;
		std::map<boost::shared_ptr<Button>, int> mPlayers[2];
		bool mShowHome;
		int mHumanTeam;
		boost::shared_ptr<Button> mToggleButtons[2];
		boost::shared_ptr<Button> mTeamLabels[2];
		std::vector<boost::shared_ptr<Button>> mPlayerLabels[2];
		std::vector<boost::shared_ptr<Button>> mFormationButtons[2];
		std::string mFormationNumbers[2];
		std::vector<boost::shared_ptr<Button>> mSkillLabels[2];
};

}

#endif



