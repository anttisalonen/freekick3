#include "AIPlayStates.h"
#include "PlayerAIController.h"

AIState::AIState(Player* p, PlayerAIController* m)
	: PlayerController(p),
	mMainAI(m)
{
}

std::shared_ptr<PlayerAction> AIState::switchState(std::shared_ptr<AIState> newstate, double time)
{
	mMainAI->setNewPlayState(newstate);
	return newstate->act(time);
}


