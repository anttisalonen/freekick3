#ifndef SOCCER_PRESETLEAGUESCREEN_H
#define SOCCER_PRESETLEAGUESCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Team.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamBrowser.h"

namespace Soccer {

class PresetLeagueScreen : public TeamBrowser {
	public:
		PresetLeagueScreen(boost::shared_ptr<ScreenManager> sm);
		bool enteringLeague(boost::shared_ptr<League> p);
		bool canClickDone();
		void clickedDone();
		const std::string& getName() const;
		bool clickingOnTeam(boost::shared_ptr<Team> p);

	private:
		static const std::string ScreenName;
};

}

#endif


