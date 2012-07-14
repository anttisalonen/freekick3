#include <iostream>
#include <algorithm>

#include "common/Math.h"

#include "match/PlayerActions.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"
#include "match/Player.h"

void IdlePA::applyPlayerAction(Match& match, Player& p, double time)
{
	AbsVector3 v = p.getVelocity();
	p.setVelocity(AbsVector3(0.0f, 0.0f, v.v.z));
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
	if(!p.standing() || p.isAirborne())
		return;
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
	if(!MatchHelpers::ballInHeadingHeight(p)) {
		v.v *= p.getMaximumShotPower();
	}
	else {
		v.v *= p.getMaximumHeadingPower();
		std::cout << "Header!\n";
	}

	int failpoints = match.kickBall(&p, v);
	AbsVector3 vel = p.getVelocity();
	p.setVelocity(AbsVector3(0.0f, 0.0f, vel.v.z));

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

TacklePA::TacklePA(const AbsVector3& v)
	: mDiff(v)
{
}

void TacklePA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(!p.standing() || p.isAirborne())
		return;
	mDiff.v.z = 0.0f;
	if(mDiff.v.length() < 0.1f)
		return;
	AbsVector3 v(mDiff.v.normalized());
	p.setAcceleration(v.v * 50.0f); /* TODO: use a player skill as the coefficient */
	p.setTackling();
}

std::string TacklePA::getDescription() const
{
	return std::string("Tackle");
}

JumpToPA::JumpToPA(const AbsVector3& v)
	: mDiff(v)
{
}

void JumpToPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(!time)
		return;
	if(!p.standing() || p.isAirborne() || mDiff.v.z < 0.01f)
		return;
	if(mDiff.v.length() < 0.1f)
		return;
	AbsVector3 v(mDiff.v.normalized());
	v.v *= 3.0f / time;
	p.setAcceleration(v.v);
}

std::string JumpToPA::getDescription() const
{
	return std::string("Jump to " + std::to_string((int)mDiff.v.x) + " " + std::to_string((int)mDiff.v.y) + " " + std::to_string(mDiff.v.z));
}


