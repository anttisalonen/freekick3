#include <iostream>
#include <algorithm>

#include "PlayerActions.h"
#include "Match.h"
#include "Player.h"

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
	v.v *= p.getRunSpeed();
	p.setVelocity(v);
}

KickBallPA::KickBallPA(const AbsVector3& v, bool absolute)
	: mDiff(v),
	mAbsolute(absolute)
{
}

void KickBallPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if((p.getPosition().v - match.getBall()->getPosition().v).length() > MAX_KICK_DISTANCE) {
		std::cout << "Can't kick - too far away\n";
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
	std::cout << "Setting ball velocity to " << v.v << "\n";
	match.kickBall(p, v);
}


