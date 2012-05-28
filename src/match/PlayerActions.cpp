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
	mDiff.v.z = 0.0f;
	if(mDiff.v.length() < 0.1f)
		return;
	AbsVector3 v(mDiff.v.normalized());
	p.setAcceleration(v.v * 50.0f); /* TODO: use a player skill as the coefficient */
}

KickBallPA::KickBallPA(const AbsVector3& v, Player* passtgt, bool absolute)
	: mDiff(v),
	mPassTarget(passtgt),
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
	v.v *= p.getMaximumShotPower();
	int failpoints = match.kickBall(&p, v);
	p.setVelocity(AbsVector3());

	if(failpoints == 0) {
		match.getTeam(0)->setPlayerReceivingPass(nullptr);
		match.getTeam(1)->setPlayerReceivingPass(nullptr);
		if(mPassTarget && mPassTarget->getTeam() == p.getTeam()) {
			p.getTeam()->setPlayerReceivingPass(mPassTarget);
		}
	}
}

void GrabBallPA::applyPlayerAction(Match& match, Player& p, double time)
{
	match.grabBall(&p);
}


