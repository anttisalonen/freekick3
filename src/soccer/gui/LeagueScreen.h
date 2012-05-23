#ifndef SOCCER_LEAGUESCREEN_H
#define SOCCER_LEAGUESCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/League.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

class LeagueScreen : public Screen {
	public:
		LeagueScreen(std::shared_ptr<ScreenManager> sm, std::shared_ptr<StatefulLeague> l);
		void buttonPressed(std::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		MatchResult playMatch(const Match& m);
		void drawTable();
		static const std::string ScreenName;
		std::shared_ptr<StatefulLeague> mLeague;
		std::shared_ptr<Button> mNextButton;
		std::vector<std::shared_ptr<Button>> mTableLabels;
};

}

#endif



