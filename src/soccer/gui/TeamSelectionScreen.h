#ifndef SOCCER_TEAMSELECTIONSCREEN_H
#define SOCCER_TEAMSELECTIONSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Team.h"
#include "soccer/Tournament.h"

#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamBrowser.h"

namespace Soccer {

class TeamSelectionScreen : public Screen {
	public:
		TeamSelectionScreen(boost::shared_ptr<ScreenManager> sm,
				const std::vector<boost::shared_ptr<Team>>& teams,
				std::unique_ptr<TournamentConfig> tc);
		void buttonPressed(boost::shared_ptr<Button> button);
		virtual const std::string& getName() const;

	private:
		bool canClickDone() const;
		void teamClicked(boost::shared_ptr<Team> p);
		void clickedDone();
		void updatePlayButton();

		static const std::string ScreenName;

		std::map<boost::shared_ptr<Team>, TeamSelection> mTeams;
		std::map<std::string, boost::shared_ptr<Team>> mButtons;
		boost::shared_ptr<Button> mPlayButton;
		std::unique_ptr<TournamentConfig> mTournamentConfig;
};

}

#endif


