#ifndef SOCCER_LEAGUESCREEN_H
#define SOCCER_LEAGUESCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/League.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

class LeagueScreen : public Screen {
	public:
		LeagueScreen(std::shared_ptr<ScreenManager> sm, std::shared_ptr<StatefulLeague> l);
		void buttonPressed(std::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		enum class LabelType {
			Table,
			Result,
		};

		MatchResult playMatch(const Match& m);
		void drawTable();
		void drawInfo();
		void addText(LabelType t, const char* text, float x, float y,
				TextAlignment align = TextAlignment::MiddleLeft);
		static const std::string ScreenName;
		std::shared_ptr<StatefulLeague> mLeague;
		const float mTextSize;
		std::shared_ptr<Button> mResultButton;
		std::vector<std::shared_ptr<Button>> mTableLabels;
		std::vector<std::shared_ptr<Button>> mResultLabels;
		std::shared_ptr<Match> mPreviousMatch;
};

}

#endif



