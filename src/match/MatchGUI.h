#ifndef MATCHGUI_H
#define MATCHGUI_H

#include "match/Clock.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"

class MatchGUI {
	public:
		inline MatchGUI(boost::shared_ptr<Match> match);
		virtual ~MatchGUI() { }
		virtual bool play() = 0;
	protected:
		inline bool progressMatch(double frameTime);
		boost::shared_ptr<Match> mMatch;

	private:
		Countdown mHalfTimeTimer;
};

MatchGUI::MatchGUI(boost::shared_ptr<Match> match)
	: mMatch(match),
	mHalfTimeTimer(1.0f)
{
}

bool MatchGUI::progressMatch(double frameTime)
{
	if(!playing(mMatch->getMatchHalf()) &&
			mMatch->getMatchHalf() != MatchHalf::NotStarted &&
			mMatch->getMatchHalf() != MatchHalf::HalfTimePauseEnd &&
			MatchHelpers::playersOnPause(*mMatch)) {
		mHalfTimeTimer.doCountdown(frameTime);
		if(mHalfTimeTimer.checkAndRewind()) {
			if(mMatch->matchOver()) {
				return false;
			}
			else {
				mMatch->setMatchHalf(MatchHalf::HalfTimePauseEnd);
			}
		}
	}
	return true;
}

#endif

