#ifndef PLAYERAICONTROLLER_H
#define PLAYERAICONTROLLER_H

#include "PlayerController.h"
#include "Distance.h"

class PlayerAIController : public PlayerController {
	public:
		PlayerAIController(Player* p);
		std::shared_ptr<PlayerAction> act();
	private:
		std::shared_ptr<PlayerAction> createMoveActionTo(const AbsVector3& pos) const;
};

#endif

