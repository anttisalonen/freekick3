#include <iostream>
#include <algorithm>

#include "common/Math.h"

#include "match/PlayerActions.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"
#include "match/Player.h"

using Common::Vector3;

void IdlePA::applyPlayerAction(Match& match, Player& p, double time)
{
	Vector3 v = p.getVelocity();
	p.setVelocity(Vector3(0.0f, 0.0f, v.z));
	return;
}

std::string IdlePA::getDescription() const
{
	return std::string("Idle");
}

RunToPA::RunToPA(const Vector3& v)
	: mDiff(v)
{
}

void RunToPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(!p.standing() || p.isAirborne())
		return;
	mDiff.z = 0.0f;
	if(mDiff.length() < 0.1f)
		return;
	Vector3 v(mDiff.normalized());
	p.setAcceleration(v * 50.0f); /* TODO: use a player skill as the coefficient */
}

std::string RunToPA::getDescription() const
{
	return std::string("Run to " + std::to_string((int)mDiff.x) + " " + std::to_string((int)mDiff.y));
}

KickBallPA::KickBallPA(const Vector3& v, Player* passtgt, bool absolute)
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
		mDiff = mDiff - p.getPosition();
	}
	if(mDiff.length() > 1.0f)
		mDiff.normalize();

	Vector3 v(mDiff);
	printf("Kicking ball with %d%% power\n", (int)(v.length() * 100));
	if(!MatchHelpers::ballInHeadingHeight(p)) {
		v *= p.getMaximumShotPower();
	}
	else {
		v *= p.getMaximumHeadingPower();
		std::cout << "Header!\n";
	}

	int failpoints = match.kickBall(&p, v);
	Vector3 vel = p.getVelocity();
	p.setVelocity(Vector3(0.0f, 0.0f, vel.z));

	if(failpoints == 0) {
		match.getTeam(0)->setPlayerReceivingPass(nullptr);
		match.getTeam(1)->setPlayerReceivingPass(nullptr);
		if(mPassTarget && mPassTarget->getTeam() == p.getTeam()) {
			p.getTeam()->setPlayerReceivingPass(mPassTarget);
		}
		match.setGoalScorer(&p);
	}
}

std::string KickBallPA::getDescription() const
{
	std::stringstream ss;
	ss << "Kick ball " << mDiff;
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

TacklePA::TacklePA(const Vector3& v)
	: mDiff(v)
{
}

void TacklePA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(!p.standing() || p.isAirborne())
		return;
	mDiff.z = 0.0f;
	if(mDiff.length() < 0.1f)
		return;
	Vector3 v(mDiff.normalized());
	p.setAcceleration(v * 50.0f); /* TODO: use a player skill as the coefficient */
	p.setTackling();
}

std::string TacklePA::getDescription() const
{
	return std::string("Tackle");
}

JumpToPA::JumpToPA(const Vector3& v)
	: mDiff(v)
{
}

void JumpToPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(!time)
		return;
	if(!p.standing() || p.isAirborne() || mDiff.z < 0.01f)
		return;
	if(mDiff.length() < 0.1f)
		return;
	Vector3 v(mDiff.normalized());
	v *= 3.0f / time;
	p.setAcceleration(v);
}

std::string JumpToPA::getDescription() const
{
	return std::string("Jump to " + std::to_string((int)mDiff.x) + " " + std::to_string((int)mDiff.y) + " " + std::to_string(mDiff.z));
}


