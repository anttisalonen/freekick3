#ifndef AIPLAYSTATES_H
#define AIPLAYSTATES_H

#include <memory>
#include <string>

#include "match/Clock.h"
#include "match/Player.h"
#include "match/PlayerController.h"
#include "match/Match.h"

class AIState;

class AIPlayController : public PlayerController {
	public:
		AIPlayController(Player* p);
		std::shared_ptr<PlayerAction> act(double time);
		std::shared_ptr<PlayerAction> switchState(std::shared_ptr<AIState> newstate, double time);
		void setNewState(std::shared_ptr<AIState> newstate);
		const std::string& getDescription() const;
		std::shared_ptr<PlayerAction> actOnRestart(double time);
		void matchHalfChanged(MatchHalf m);
	private:
		std::shared_ptr<AIState> mCurrentState;
};

class AIState {
	public:
		AIState(Player* p, AIPlayController* m);
		virtual ~AIState() { }
		virtual std::shared_ptr<PlayerAction> actOnBall(double time) = 0;
		virtual std::shared_ptr<PlayerAction> actNearBall(double time) = 0;
		virtual std::shared_ptr<PlayerAction> actOffBall(double time) = 0;
		const std::string& getDescription() const;
		virtual void matchHalfChanged(MatchHalf m) { }
	protected:
		std::shared_ptr<PlayerAction> switchState(std::shared_ptr<AIState> newstate, double time);
		void setNewState(std::shared_ptr<AIState> newstate);
		Player* mPlayer;
		AIPlayController* mPlayController;
		std::string mDescription;
};

class AIGoalkeeperState : public AIState {
	public:
		AIGoalkeeperState(Player* p, AIPlayController* m);
		std::shared_ptr<PlayerAction> actOnBall(double time);
		std::shared_ptr<PlayerAction> actNearBall(double time);
		std::shared_ptr<PlayerAction> actOffBall(double time);
		void matchHalfChanged(MatchHalf m) override;
	private:
		void setPivotPoint();
		AbsVector3 mPivotPoint;
		float mDistanceFromPivot;
		Countdown mHoldBallTimer;
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

