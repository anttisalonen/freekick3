#ifndef SOCCER_LEAGUESCREEN_H
#define SOCCER_LEAGUESCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/League.h"

#include "soccer/gui/CompetitionScreen.h"
#include "soccer/gui/TeamTacticsScreen.h"

namespace Soccer {

class LeagueScreen : public CompetitionScreen {
	public:
		LeagueScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulLeague> l);
		virtual void drawTable() override;

	private:
		void addTableText(const char* text, float x, float y,
				TextAlignment align = TextAlignment::MiddleLeft,
				Common::Color col = Common::Color::White);

		std::vector<boost::shared_ptr<Button>> mTableLabels;
		boost::shared_ptr<StatefulLeague> mLeague;
};

}

#endif



