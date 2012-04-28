#ifndef PLAYERAICONTROLLER_H
#define PLAYERAICONTROLLER_H

#include "PlayerController.h"

class PlayerAIController : public PlayerController {
	public:
		PlayerAIController(Player* p);
		std::shared_ptr<PlayerAction> act();
};

#endif

