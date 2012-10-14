#ifndef SOCCER_PRESETSEASONSCREEN_H
#define SOCCER_PRESETSEASONSCREEN_H

#include "soccer/Team.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/PresetLeagueScreen.h"

namespace Soccer {

class PresetSeasonScreen : public PresetLeagueScreen {
	public:
		PresetSeasonScreen(boost::shared_ptr<ScreenManager> sm);

	protected:
		virtual bool enteringCountry(boost::shared_ptr<LeagueSystem> p) override;
		virtual bool enteringLeague(boost::shared_ptr<League> p) override;
		virtual bool canClickDone() override;
		virtual void clickedDone() override;
		virtual const std::string& getName() const override;
		virtual bool clickingOnTeam(boost::shared_ptr<Team> p) override;

	private:
		static const std::string ScreenName;

		boost::shared_ptr<League> mLeague;
		boost::shared_ptr<LeagueSystem> mCountry;
		boost::shared_ptr<Team> mOwnTeam;
};

}

#endif


