#include <stdio.h>

#include "match/Player.h"
#include "match/ai/PlayerAIController.h"
#include "match/Match.h"
#include "match/Team.h"

Player::Player(Match* match, Team* team, const Soccer::Player& p,
		ShirtNumber sn)
	: MatchEntity(match, match->convertRelativeToAbsoluteVector(team->getPausePosition())),
	Soccer::Player(p),
	mTeam(team),
	mBallKickedTimer(1.0f - p.getSkills().BallControl * 0.5f),
	mShirtNumber(sn)
{
	mAIController = new PlayerAIController(this);
	setAIControlled();
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

std::shared_ptr<PlayerAction> Player::act(double time)
{
	return mController->act(time);
}

const RelVector3& Player::getHomePosition() const
{
	return mHomePosition;
}

void Player::setHomePosition(const RelVector3& p)
{
	printf("Set home position to %3.2f, %3.2f\n", p.v.x, p.v.y);
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

double Player::getMaximumKickPower() const
{
	return 40.0f * ((1.0f + mSkills.KickPower) * 0.5f);
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
	return !mBallKickedTimer.running();
}

void Player::update(float time)
{
	MatchEntity::update(time);
	if(mVelocity.v.length() > getRunSpeed()) {
		mVelocity.v.normalize();
		mVelocity.v *= getRunSpeed();
	}
	mBallKickedTimer.doCountdown(time);
	mBallKickedTimer.check();
}

void Player::setPlayerTactics(const PlayerTactics& t)
{
	mTactics = t;
}

const PlayerTactics& Player::getTactics() const
{
	return mTactics;
}

void Player::matchHalfChanged(MatchHalf m)
{
	if(m == MatchHalf::HalfTimePauseEnd) {
		mHomePosition.v.y = -mHomePosition.v.y;
	}

	if(mController)
		mController->matchHalfChanged(m);
	if(mController != mAIController)
		mAIController->matchHalfChanged(m);
}

