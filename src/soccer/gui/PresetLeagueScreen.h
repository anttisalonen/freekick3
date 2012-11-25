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
		virtual bool enteringLeague(boost::shared_ptr<League> p) override;
		virtual bool canClickDone() override;
		virtual void clickedDone() override;
		virtual const std::string& getName() const override;
		virtual bool clickingOnTeam(boost::shared_ptr<Team> p) override;

	private:
		static const std::string ScreenName;
		boost::shared_ptr<League> mLeague;
};

}

#endif


