#include <stdio.h>

#include "Player.h"
#include "PlayerAIController.h"
#include "Match.h"
#include "Team.h"

Player::Player(Match* match, Team* team, int shirtnumber, bool goalkeeper)
	: MatchEntity(match, match->convertRelativeToAbsoluteVector(team->getPausePosition())),
	mTeam(team),
	mShirtNumber(shirtnumber),
	mGoalkeeper(goalkeeper),
	mBallKickedTimer(1.0f)
{
	if(!team->isFirst()) {
		mSkills.KickPower = 0.5f;
		mSkills.RunSpeed = 0.5f;
		mSkills.BallControl = 0.5f;
	}
	mAIController = new PlayerAIController(this);
	setAIControlled();
}

Player::~Player()
{
	delete mAIController;
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

bool Player::isGoalkeeper() const
{
	return mGoalkeeper;
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

int Player::getShirtNumber() const
{
	return mShirtNumber;
}

const PlayerSkills& Player::getSkills() const
{
	return mSkills;
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

