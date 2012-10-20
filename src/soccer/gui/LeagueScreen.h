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
		LeagueScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulLeague> l,
				bool onlyOneRound = false);
		virtual void drawTable() override;
		static void drawTable(Screen& scr, std::vector<boost::shared_ptr<Button>>& labels, const StatefulLeague& l, float x, float y);
		static void addTableText(Screen& scr, const char* text, float x, float y,
				TextAlignment align, Common::Color col,
				std::vector<boost::shared_ptr<Button>>& labels);

	protected:
		virtual void saveCompetition(boost::archive::binary_oarchive& oa) const override;

	private:
		std::vector<boost::shared_ptr<Button>> mTableLabels;
		boost::shared_ptr<StatefulLeague> mLeague;
};

}

#endif



