#ifndef SOCCER_PRESETTOURNAMENTSCREEN_H
#define SOCCER_PRESETTOURNAMENTSCREEN_H

#include "soccer/gui/Screen.h"

namespace Soccer {

class PresetTournamentScreen : public Screen {
	public:
		PresetTournamentScreen(boost::shared_ptr<ScreenManager> sm);
		void buttonPressed(boost::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		std::vector<boost::shared_ptr<Team>> getTournamentTeams(const TournamentConfig& tc);
		std::map<boost::shared_ptr<Button>, TournamentConfig> mTournamentButtons;

		static const std::string ScreenName;
};

}

#endif

