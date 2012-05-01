#include "AIPlayStates.h"
#include "PlayerActions.h"

AIGoalkeeperState::AIGoalkeeperState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::actOnBall(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::actNearBall(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::actOffBall(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

