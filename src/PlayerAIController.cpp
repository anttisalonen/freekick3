#include "PlayerAIController.h"
#include "Player.h"
#include "PlayerActions.h"

PlayerAIController::PlayerAIController(Player* p)
	: PlayerController(p)
{
}

std::shared_ptr<PlayerAction> PlayerAIController::act()
{
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

