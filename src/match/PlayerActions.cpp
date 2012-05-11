#include <iostream>
#include <algorithm>

#include "match/PlayerActions.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"
#include "match/Player.h"

void IdlePA::applyPlayerAction(Match& match, Player& p, double time)
{
	p.setVelocity(AbsVector3());
	return;
}

RunToPA::RunToPA(const AbsVector3& v)
	: mDiff(v)
{
}

void RunToPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(mDiff.v.length() < 0.1f)
		return;
	AbsVector3 v(mDiff.v.normalized());
	p.setAcceleration(v.v * 50.0f); /* TODO: use a player skill as the coefficient */
}

KickBallPA::KickBallPA(const AbsVector3& v, bool absolute)
	: mDiff(v),
	mAbsolute(absolute)
{
}

void KickBallPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(!MatchHelpers::canKickBall(p)) {
		return;
	}
	if(mAbsolute) {
		mAbsolute = false;
		mDiff.v = mDiff.v - p.getPosition().v;
	}
	if(mDiff.v.length() > 1.0f)
		mDiff.v.normalize();
	AbsVector3 v(mDiff);
	v.v *= p.getMaximumKickPower();
	match.kickBall(&p, v);
	p.setVelocity(AbsVector3());
}

void GrabBallPA::applyPlayerAction(Match& match, Player& p, double time)
{
	match.grabBall(&p);
}


