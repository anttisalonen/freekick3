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
		void setNewPlayState(std::shared_ptr<PlayerController> p);
	protected:
		std::shared_ptr<PlayerAction> createMoveActionTo(const AbsVector3& pos) const;
	private:
		Countdown mKickInTimer;
		std::shared_ptr<PlayerController> mCurrentPlayState;
};

#endif

