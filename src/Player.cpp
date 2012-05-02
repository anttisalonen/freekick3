#include <stdio.h>

#include "Player.h"
#include "PlayerAIController.h"
#include "Match.h"

Player::Player(Match* match, Team* team, bool goalkeeper)
	: MatchEntity(match, match->convertRelativeToAbsoluteVector(team->getPausePosition())),
	mTeam(team),
	mGoalkeeper(goalkeeper),
	mBallKickedTimer(1.0f)
{
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
	return 8.0f;
}

double Player::getMaximumKickPower() const
{
	return 40.0f;
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

