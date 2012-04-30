#ifndef AIPLAYSTATES_H
#define AIPLAYSTATES_H

#include <memory>

#include "Player.h"
#include "PlayerController.h"
#include "Match.h"

class AIGoalkeeperState : public PlayerController {
	public:
		AIGoalkeeperState(Player* p);
		std::shared_ptr<PlayerAction> act(double time);
};

class AIDefendState : public PlayerController {
	public:
		AIDefendState(Player* p);
		std::shared_ptr<PlayerAction> act(double time);
};

#endif

