#ifndef AIPLAYSTATES_H
#define AIPLAYSTATES_H

#include <memory>

#include "Player.h"
#include "PlayerController.h"
#include "Match.h"

class AIState : public PlayerController {
	public:
		AIState(Player* p, PlayerAIController* m);
	protected:
		std::shared_ptr<PlayerAction> switchState(std::shared_ptr<AIState> newstate, double time);
		PlayerAIController* mMainAI;
};

class AIGoalkeeperState : public AIState {
	public:
		AIGoalkeeperState(Player* p, PlayerAIController* m);
		std::shared_ptr<PlayerAction> act(double time);
};

class AIDefendState : public AIState {
	public:
		AIDefendState(Player* p, PlayerAIController* m);
		std::shared_ptr<PlayerAction> act(double time);
};

class AIKickBallState : public AIState {
	public:
		AIKickBallState(Player* p, PlayerAIController* m);
		std::shared_ptr<PlayerAction> act(double time);
	protected:
		double getBestPassTarget(Player* p);
		double getBestDribbleTarget(AbsVector3* v);
		double getBestShootTarget(AbsVector3* v);
};

class AIOffensiveState : public AIState {
	public:
		AIOffensiveState(Player* p, PlayerAIController* m);
		std::shared_ptr<PlayerAction> act(double time);
};


#endif

