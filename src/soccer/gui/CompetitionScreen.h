#ifndef SOCCER_COMPETITIONSCREEN_H
#define SOCCER_COMPETITIONSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "soccer/Competition.h"

#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamTacticsScreen.h"

namespace Soccer {

class CompetitionScreen : public Screen {
	public:
		CompetitionScreen(boost::shared_ptr<ScreenManager> sm, const std::string& name,
				boost::shared_ptr<StatefulCompetition> l,
				bool onlyOneRound = false);
		virtual ~CompetitionScreen() { }
		virtual void buttonPressed(boost::shared_ptr<Button> button) override;
		virtual const std::string& getName() const override;
		virtual void drawTable() { }

	protected:
		void updateScreenElements();
		Common::Color mMyTeamColor;
		virtual void saveCompetition(boost::archive::binary_oarchive& oa) const = 0;

	private:
		enum class LabelType {
			Table,
			Result,
		};

		void drawInfo();
		void addResultText(const char* text, float x, float y,
				TextAlignment align = TextAlignment::MiddleLeft,
				Common::Color col = Common::Color::White);
		bool allRoundMatchesPlayed() const;
		void updateRoundMatches();
		bool playNextMatch(bool display);
		void addMatchLabels(const Match& m, float xp, float yp);
		bool shouldShowSkipButton() const;
		void saveCompetition() const;
		void updateNextRoundButton();
		void skipMatches();

		const std::string mCompetitionName;
		const std::string mScreenName;
		boost::shared_ptr<StatefulCompetition> mCompetition;
		const float mTextSize;
		boost::shared_ptr<Button> mSkipButton;
		boost::shared_ptr<Button> mResultButton;
		boost::shared_ptr<Button> mMatchButton;
		boost::shared_ptr<Button> mNextRoundButton;
		std::vector<boost::shared_ptr<Button>> mResultLabels;
		std::vector<boost::shared_ptr<Match>> mRoundMatches;

		bool mOneRound;
};

}

#endif



