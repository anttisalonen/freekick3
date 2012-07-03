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
		static const std::string ScreenName;
		Match& mMatch;
		std::vector<boost::shared_ptr<Button>> mPlayerButtons;
		TeamTacticsScreenOwner& mTtso;
		int mChosenplnum;
		std::map<boost::shared_ptr<Button>, int> mPlrlabels[2];
		bool mShowHome;
		int mHumanTeam;
		boost::shared_ptr<Button> mHomeButton;
		boost::shared_ptr<Button> mAwayButton;
		boost::shared_ptr<Button> mHomeLabel;
		boost::shared_ptr<Button> mAwayLabel;
};

}

#endif



