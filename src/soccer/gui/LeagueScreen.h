#ifndef SOCCER_LEAGUESCREEN_H
#define SOCCER_LEAGUESCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/League.h"

#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamTacticsScreen.h"

namespace Soccer {

class LeagueScreen : public Screen, public TeamTacticsScreenOwner {
	public:
		LeagueScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulLeague> l);
		void buttonPressed(boost::shared_ptr<Button> button);
		const std::string& getName() const;
		void TeamTacticsScreenFinished(int playernum);

	private:
		enum class LabelType {
			Table,
			Result,
		};

		MatchResult playMatch(bool display, Match& m);
		void drawTable();
		void drawInfo();
		void addText(LabelType t, const char* text, float x, float y,
				TextAlignment align = TextAlignment::MiddleLeft,
				Common::Color col = Common::Color::White);
		bool allRoundMatchesPlayed() const;
		void updateRoundMatches();
		bool playNextMatch(bool display);
		void addMatchLabels(const Match& m, float xp, float yp);
		void updateScreenElements();
		bool shouldShowSkipButton() const;
		void saveLeague() const;

		static const std::string ScreenName;
		boost::shared_ptr<StatefulLeague> mLeague;
		const float mTextSize;
		boost::shared_ptr<Button> mSkipButton;
		boost::shared_ptr<Button> mResultButton;
		boost::shared_ptr<Button> mMatchButton;
		std::vector<boost::shared_ptr<Button>> mTableLabels;
		std::vector<boost::shared_ptr<Button>> mResultLabels;
		std::vector<boost::shared_ptr<Match>> mRoundMatches;
		Common::Color mMyTeamColor;
};

}

#endif



