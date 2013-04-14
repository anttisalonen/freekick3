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
		virtual bool drawTable() { return false; }

		static void addResultLabels(int a, int b, float xp, float yp,
				float fontsize, Screen& scr, std::vector<boost::shared_ptr<Button>>& labels,
				const char* suffix = NULL);
		static float addMatchLabels(const Match& m, float xp, float yp, float fontsize,
				Screen& scr,
				std::vector<boost::shared_ptr<Button>>& labels, bool rowsAvailable);

	protected:
		void updateScreenElements();
		virtual void saveCompetition(boost::archive::binary_oarchive& oa) const = 0;

	private:
		enum class LabelType {
			Table,
			Result,
		};

		void drawInfo(bool drewtable);
		bool allRoundMatchesPlayed() const;
		void updateRoundMatches();
		bool playNextMatch(bool display);
		float addMatchLabels(const Match& m, float xp, float yp);
		bool shouldShowSkipButton() const;
		void saveCompetition() const;
		void updateNextRoundButton();
		void skipMatches();
		void nextRound();
		void skip();
		void result();
		void match();

		const std::string mCompetitionName;
		const std::string mScreenName;
		boost::shared_ptr<StatefulCompetition> mCompetition;
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



