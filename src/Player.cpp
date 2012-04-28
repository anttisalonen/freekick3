#include <stdio.h>

#include "Player.h"
#include "PlayerAIController.h"
#include "Match.h"

Player::Player(Match* match, Team* team)
	: mMatch(match),
	mTeam(team),
	mPosition(match->convertRelativeToAbsoluteVector(team->getPausePosition()))
{
	mController = std::shared_ptr<PlayerController>(new PlayerAIController(this));
}

std::shared_ptr<PlayerAction> Player::act()
{
	return mController->act();
}

const Match* Player::getMatch() const
{
	return mMatch;
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

float Player::getRunSpeed() const
{
	return 8.0f;
}

void Player::move(const AbsVector3& v)
{
	mPosition.v += v.v;
}

const AbsVector3& Player::getPosition() const
{
	return mPosition;
}

