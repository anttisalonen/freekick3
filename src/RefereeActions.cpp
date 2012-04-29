#include "RefereeActions.h"
#include "Match.h"
#include "Referee.h"

void IdleRA::applyRefereeAction(Match& match, const Referee& p, double time)
{
	return;
}

ChangeMatchHalfRA::ChangeMatchHalfRA(MatchHalf h)
	: mMatchHalf(h)
{
}

void ChangeMatchHalfRA::applyRefereeAction(Match& match, const Referee& p, double time)
{
	match.setMatchHalf(mMatchHalf);
}

ChangePlayStateRA::ChangePlayStateRA(PlayState h)
	: mPlayState(h)
{
}

void ChangePlayStateRA::applyRefereeAction(Match& match, const Referee& p, double time)
{
	match.setPlayState(mPlayState);
}



