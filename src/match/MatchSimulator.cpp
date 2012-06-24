#include "match/MatchSimulator.h"

MatchSimulator::MatchSimulator(boost::shared_ptr<Match> match,
		int ticksPerSec,
		bool randomise)
	: MatchGUI(match),
	mFixedFrameTime(0.0f),
	mRandomise(randomise)
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
		/* TODO: remove this duplicate code (shared with MatchSDLGUI) */
		if(mFixedFrameTime && mRandomise) {
			double add = rand() / (double)RAND_MAX;
			add -= 0.5f;
			add *= 0.01f * mFixedFrameTime;
			frameTime += add;
		}
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

