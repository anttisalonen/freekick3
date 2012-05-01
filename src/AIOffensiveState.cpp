#include "AIPlayStates.h"
#include "PlayerActions.h"

AIOffensiveState::AIOffensiveState(Player* p, PlayerAIController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIOffensiveState::act(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}


