#ifndef AIPLAYSTATES_H
#define AIPLAYSTATES_H

#include <memory>

#include "Player.h"
#include "PlayerController.h"
#include "Match.h"

class AIState;

class AIPlayController : public PlayerController {
	public:
		AIPlayController(Player* p);
		std::shared_ptr<PlayerAction> act(double time);
		std::shared_ptr<PlayerAction> switchState(std::shared_ptr<AIState> newstate, double time);
		void setNewState(std::shared_ptr<AIState> newstate);
	private:
		std::shared_ptr<AIState> mCurrentState;
};

class AIState {
	public:
		AIState(Player* p, AIPlayController* m);
		virtual std::shared_ptr<PlayerAction> actOnBall(double time) = 0;
		virtual std::shared_ptr<PlayerAction> actNearBall(double time) = 0;
		virtual std::shared_ptr<PlayerAction> actOffBall(double time) = 0;
	protected:
		std::shared_ptr<PlayerAction> switchState(std::shared_ptr<AIState> newstate, double time);
		void setNewState(std::shared_ptr<AIState> newstate);
		Player* mPlayer;
		AIPlayController* mPlayController;
};

class AIGoalkeeperState : public AIState {
	public:
		AIGoalkeeperState(Player* p, AIPlayController* m);
		std::shared_ptr<PlayerAction> actOnBall(double time);
		std::shared_ptr<PlayerAction> actNearBall(double time);
		std::shared_ptr<PlayerAction> actOffBall(double time);
};

class AIDefendState : public AIState {
	public:
		AIDefendState(Player* p, AIPlayController* m);
		std::shared_ptr<PlayerAction> actOnBall(double time);
		std::shared_ptr<PlayerAction> actNearBall(double time);
		std::shared_ptr<PlayerAction> actOffBall(double time);
};

class AIKickBallState : public AIState {
	public:
		AIKickBallState(Player* p, AIPlayController* m);
		std::shared_ptr<PlayerAction> actOnBall(double time);
		std::shared_ptr<PlayerAction> actNearBall(double time);
		std::shared_ptr<PlayerAction> actOffBall(double time);
};

class AIOffensiveState : public AIState {
	public:
		AIOffensiveState(Player* p, AIPlayController* m);
		std::shared_ptr<PlayerAction> actOnBall(double time);
		std::shared_ptr<PlayerAction> actNearBall(double time);
		std::shared_ptr<PlayerAction> actOffBall(double time);
};


#endif

