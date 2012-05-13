#ifndef MATCHGUI_H
#define MATCHGUI_H

#include "match/Match.h"

class MatchGUI {
	public:
		inline MatchGUI(std::shared_ptr<Match> match);
		virtual ~MatchGUI() { }
		virtual bool play() = 0;
	protected:
		std::shared_ptr<Match> mMatch;
};

MatchGUI::MatchGUI(std::shared_ptr<Match> match)
	: mMatch(match)
{
}

#endif

