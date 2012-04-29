#ifndef PLAYERAICONTROLLER_H
#define PLAYERAICONTROLLER_H

#include "PlayerController.h"
#include "Distance.h"
#include "Clock.h"

class PlayerAIController : public PlayerController {
	public:
		PlayerAIController(Player* p);
		std::shared_ptr<PlayerAction> act(double time);
	private:
		std::shared_ptr<PlayerAction> createMoveActionTo(const AbsVector3& pos) const;
		Countdown mKickInTimer;
};

#endif

