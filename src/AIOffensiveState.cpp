#include "AIPlayStates.h"
#include "PlayerActions.h"

AIOffensiveState::AIOffensiveState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIOffensiveState::actOnBall(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

std::shared_ptr<PlayerAction> AIOffensiveState::actNearBall(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

std::shared_ptr<PlayerAction> AIOffensiveState::actOffBall(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}


