#ifndef SOCCER_PRESETLEAGUESCREEN_H
#define SOCCER_PRESETLEAGUESCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/Team.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamBrowser.h"

namespace Soccer {

class PresetLeagueScreen : public TeamBrowser {
	public:
		PresetLeagueScreen(std::shared_ptr<ScreenManager> sm);
		bool enteringLeague(std::shared_ptr<League> p);
		bool canClickDone();
		void clickedDone();
		const std::string& getName() const;
		bool clickingOnTeam(std::shared_ptr<Team> p);

	private:
		static const std::string ScreenName;
};

}

#endif


