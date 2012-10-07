#ifndef SOCCER_PRESETCUPSCREEN_H
#define SOCCER_PRESETCUPSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Team.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamBrowser.h"

namespace Soccer {

class PresetCupScreen : public TeamBrowser {
	public:
		PresetCupScreen(boost::shared_ptr<ScreenManager> sm);
		bool enteringCountry(boost::shared_ptr<LeagueSystem> p);
		bool canClickDone();
		void clickedDone();
		const std::string& getName() const;
		bool clickingOnTeam(boost::shared_ptr<Team> p);

	private:
		static const std::string ScreenName;
};

}

#endif


