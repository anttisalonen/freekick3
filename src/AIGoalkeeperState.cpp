#include "AIPlayStates.h"
#include "PlayerActions.h"

AIGoalkeeperState::AIGoalkeeperState(Player* p)
	: PlayerController(p)
{
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::act(double time)
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

