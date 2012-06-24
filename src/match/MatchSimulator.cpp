#include "match/MatchSimulator.h"

MatchSimulator::MatchSimulator(boost::shared_ptr<Match> match,
		int ticksPerSec)
	: MatchGUI(match),
	mFixedFrameTime(0.0f)
{
	if(ticksPerSec) {
		mFixedFrameTime = 1.0f / ticksPerSec;
	}
}

bool MatchSimulator::play()
{
	double prevTime = Clock::getTime();
	while(1) {
		double newTime = Clock::getTime();
		double frameTime = mFixedFrameTime ? mFixedFrameTime : newTime - prevTime;
		prevTime = newTime;

		mMatch->update(frameTime);

		if(!progressMatch(frameTime))
			break;
	}
	if(mMatch->matchOver()) {
		Soccer::MatchResult mres(mMatch->getScore(1), mMatch->getScore(0));
		mMatch->setResult(mres);
		return true;
	}
	return false;
}

