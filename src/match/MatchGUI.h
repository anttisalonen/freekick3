#ifndef MATCHGUI_H
#define MATCHGUI_H

#include "match/Match.h"

class MatchGUI {
	public:
		inline MatchGUI(boost::shared_ptr<Match> match);
		virtual ~MatchGUI() { }
		virtual bool play() = 0;
	protected:
		boost::shared_ptr<Match> mMatch;
};

MatchGUI::MatchGUI(boost::shared_ptr<Match> match)
	: mMatch(match)
{
}

#endif

