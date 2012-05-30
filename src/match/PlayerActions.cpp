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

std::string IdlePA::getDescription() const
{
	return std::string("Idle");
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

std::string RunToPA::getDescription() const
{
	return std::string("Run to " + std::to_string((int)mDiff.v.x) + " " + std::to_string((int)mDiff.v.y));
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

std::string KickBallPA::getDescription() const
{
	std::stringstream ss;
	ss << "Kick ball " << mDiff.v;
	return ss.str();
}

void GrabBallPA::applyPlayerAction(Match& match, Player& p, double time)
{
	match.grabBall(&p);
}

std::string GrabBallPA::getDescription() const
{
	return std::string("Grab");
}


