#include "Player.h"
#include "PlayerAIController.h"
#include "Match.h"

Player::Player(Match* match)
	: mMatch(match)
{
	mController = std::shared_ptr<PlayerController>(new PlayerAIController(this));
}

std::shared_ptr<PlayerAction> Player::act()
{
	return mController->act();
}


