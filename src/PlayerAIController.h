#ifndef PLAYERAICONTROLLER_H
#define PLAYERAICONTROLLER_H

#include "PlayerController.h"
#include "AIPlayStates.h"
#include "Distance.h"
#include "Clock.h"

class PlayerAIController : public PlayerController {
	public:
		PlayerAIController(Player* p);
		std::shared_ptr<PlayerAction> act(double time);
	protected:
		std::shared_ptr<PlayerAction> createMoveActionTo(const AbsVector3& pos) const;
	private:
		Countdown mKickInTimer;
		std::shared_ptr<AIPlayController> mPlayState;
};

#endif

