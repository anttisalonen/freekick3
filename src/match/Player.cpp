#include <stdio.h>

#include "match/Player.h"
#include "match/ai/PlayerAIController.h"
#include "match/Match.h"
#include "match/Team.h"

using Common::Vector3;

Player::Player(Match* match, Team* team, const Soccer::Player& p,
		ShirtNumber sn, const Soccer::PlayerTactics& t)
	: MatchEntity(match, false, match->convertRelativeToAbsoluteVector(team->getPausePosition())),
	Soccer::Player(p),
	mTeam(team),
	mBallKickedTimer(1.0f - p.getSkills().Tackling * 0.5f),
	mTactics(t),
	mShirtNumber(sn),
	mTacklingTimer(1.0f), /* TODO: make dependent on player skill */
	mTackledTimer(2.0f)   /* TODO: make dependent on player skill */
{
	mAIController = new PlayerAIController(this);
	setAIControlled();

	if(mShirtNumber == 1) {
		setHomePosition(RelVector3(0, -0.95f * (mTeam->isFirst() ? 1 : -1), 0));
	}
	else {
		if(mShirtNumber >= 10) {
			setHomePosition(RelVector3(mShirtNumber == 10 ? -0.1f : 0.1f,
						0.20f * (mTeam->isFirst() ? -1 : 1), 0));
		}
		else {
			int hgt = mShirtNumber > 5 ? 1 : 0;
			float widthpos = ((mShirtNumber - 2) % 4 - 1.5f) * 0.5f;
			setHomePosition(RelVector3(widthpos,
						(mTeam->isFirst() ? 1 : -1) * (-0.7f + hgt * 0.4f),
						0));
		}
	}
}

Player::~Player()
{
	delete mAIController;
}

int Player::getShirtNumber() const
{
	return mShirtNumber;
}

const PlayerAIController* Player::getAIController() const
{
	return mAIController;
}

boost::shared_ptr<PlayerAction> Player::act(double time)
{
	return mController->act(time);
}

const RelVector3& Player::getHomePosition() const
{
	return mHomePosition;
}

void Player::setHomePosition(const RelVector3& p)
{
	// printf("Set home position to %3.2f, %3.2f\n", p.x, p.y);
	mHomePosition = p;
}

Team* Player::getTeam()
{
	return mTeam;
}

const Team* Player::getTeam() const
{
	return mTeam;
}

double Player::getRunSpeed() const
{
	return 8.0f * ((1.0f + mSkills.RunSpeed) * 0.5f);
}

double Player::getMaximumShotPower() const
{
	return 30.0f + 20.0f * mSkills.ShotPower;
}

double Player::getMaximumHeadingPower() const
{
	return 10.0f + 20.0f * mSkills.Heading;
}

void Player::setController(PlayerController* c)
{
	mController = c;
}

void Player::setAIControlled()
{
	mController = mAIController;
}

bool Player::isAIControlled() const
{
	return mController == mAIController;
}

void Player::ballKicked()
{
	mBallKickedTimer.rewind();
}

bool Player::canKickBall() const
{
	return !mTackledTimer.running() && !mBallKickedTimer.running();
}

void Player::update(float time)
{
	MatchEntity::update(time);
	Vector3 planevel(mVelocity);
	planevel.z = 0.0f;
	if(planevel.length() > getRunSpeed()) {
		planevel.normalize();
		planevel *= getRunSpeed();
		mVelocity.x = planevel.x;
		mVelocity.y = planevel.y;
	}
	if(!isAirborne()) {
		mVelocity.z = std::max(0.0f, mVelocity.z);
		mPosition.z = std::max(0.0f, mPosition.z);
	}
	else {
		mVelocity.z -= 9.81f * time;
	}
	mBallKickedTimer.doCountdown(time);
	mBallKickedTimer.check();

	mTacklingTimer.doCountdown(time);
	if(mTacklingTimer.running() && mTacklingTimer.timeLeft() < 0.5f) {
		mVelocity *= 0.5f;
		mTacklingTimer.check();
	}

	mTackledTimer.doCountdown(time);
	mTackledTimer.check();
}

void Player::setPlayerTactics(const Soccer::PlayerTactics& t)
{
	mTactics = t;
}

const Soccer::PlayerTactics& Player::getTactics() const
{
	return mTactics;
}

void Player::matchHalfChanged(MatchHalf m)
{
	if(m == MatchHalf::HalfTimePauseEnd || m == MatchHalf::FullTimePauseEnd ||
			m == MatchHalf::ExtraTimeSecondHalf) {
		mHomePosition.v.x = -mHomePosition.v.x;
		mHomePosition.v.y = -mHomePosition.v.y;
	}

	if(mController)
		mController->matchHalfChanged(m);
	if(mController != mAIController)
		mAIController->matchHalfChanged(m);
}

void Player::setTackling()
{
	mTacklingTimer.rewind();
}

void Player::setTackled()
{
	mTackledTimer.rewind();
	mVelocity.zero();
	mAcceleration.zero();
}

bool Player::standing() const
{
	return !tackling() && !mTackledTimer.running();
}

bool Player::tackling() const
{
	return mTacklingTimer.running();
}

bool Player::isAirborne() const
{
	return mPosition.z > 0.05f;
}

Soccer::PlayerPosition Player::getPlayerPosition() const
{
	return mTactics.Position;
}

bool Player::isGoalkeeper() const
{
	return getPlayerPosition() == Soccer::PlayerPosition::Goalkeeper;
}


