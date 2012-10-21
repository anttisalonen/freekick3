#ifndef SOCCER_TOURNAMENTSCREEN_H
#define SOCCER_TOURNAMENTSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Tournament.h"

#include "soccer/gui/LeagueScreen.h"
#include "soccer/gui/TeamTacticsScreen.h"

namespace Soccer {

class TournamentScreen : public CompetitionScreen {
	public:
		TournamentScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulTournament> l,
				bool onlyOneRound = false);
		virtual bool drawTable() override;
		virtual void buttonPressed(boost::shared_ptr<Button> button) override;

	protected:
		virtual void saveCompetition(boost::archive::binary_oarchive& oa) const override;

	private:
		void addTableText(const char* text, float x, float y,
				TextAlignment align = TextAlignment::MiddleLeft,
				Common::Color col = Common::Color::White);
		void setScrollPositionToActiveGroup();

		std::vector<boost::shared_ptr<Button>> mTableLabels;
		boost::shared_ptr<StatefulTournament> mTournament;
		unsigned int mScrollPosition;
		unsigned int mActiveScrollPosition;
		boost::shared_ptr<Button> mScrollUpButton;
		boost::shared_ptr<Button> mScrollDownButton;
};

}

#endif



