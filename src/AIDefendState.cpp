#include "AIPlayStates.h"
#include "PlayerActions.h"

AIDefendState::AIDefendState(Player* p)
	: PlayerController(p)
{
}

std::shared_ptr<PlayerAction> AIDefendState::act(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}


