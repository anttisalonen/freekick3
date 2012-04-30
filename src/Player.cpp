#include <stdio.h>

#include "Player.h"
#include "PlayerAIController.h"
#include "Match.h"

Player::Player(Match* match, Team* team, bool goalkeeper)
	: MatchEntity(match, match->convertRelativeToAbsoluteVector(team->getPausePosition())),
	mTeam(team),
	mGoalkeeper(goalkeeper)
{
	mAIController = new PlayerAIController(this);
	setAIControlled();
}

Player::~Player()
{
	delete mAIController;
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

