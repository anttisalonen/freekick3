#ifndef AIPLAYSTATES_H
#define AIPLAYSTATES_H

#include <boost/shared_ptr.hpp>
#include <string>

#include "match/Clock.h"
#include "match/Player.h"
#include "match/PlayerController.h"
#include "match/Match.h"

class AIState;

class AIPlayController : public PlayerController {
	public:
		AIPlayController(Player* p);
		boost::shared_ptr<PlayerAction> act(double time);
		boost::shared_ptr<PlayerAction> switchState(boost::shared_ptr<AIState> newstate, double time);
		void setNewState(boost::shared_ptr<AIState> newstate);
		const std::string& getDescription() const;
		boost::shared_ptr<PlayerAction> actOnRestart(double time);
		void matchHalfChanged(MatchHalf m);
	private:
		boost::shared_ptr<AIState> mCurrentState;
};

class AIState {
	public:
		AIState(Player* p, AIPlayController* m);
		virtual ~AIState() { }
		virtual boost::shared_ptr<PlayerAction> actOnBall(double time);
		virtual boost::shared_ptr<PlayerAction> actNearBall(double time);
		virtual boost::shared_ptr<PlayerAction> actOffBall(double time) = 0;
		const std::string& getDescription() const;
		virtual void matchHalfChanged(MatchHalf m) { }
		bool checkBlockedMatchTimer(double time);
		void blockedMatch();

	protected:
		boost::shared_ptr<PlayerAction> switchState(boost::shared_ptr<AIState> newstate, double time);
		void setNewState(boost::shared_ptr<AIState> newstate);
		boost::shared_ptr<PlayerAction> gotoKickPositionOrKick(double time, const Common::Vector3& pos) const;
		boost::shared_ptr<PlayerAction> fetchAndKickBall(double time, bool kicking) const;
		Player* mPlayer;
		AIPlayController* mPlayController;
		std::string mDescription;
		Countdown mBlockedMatchTimer;
};

class AIGoalkeeperState : public AIState {
	public:
		AIGoalkeeperState(Player* p, AIPlayController* m);
		boost::shared_ptr<PlayerAction> actOnBall(double time) override;
		boost::shared_ptr<PlayerAction> actNearBall(double time) override;
		boost::shared_ptr<PlayerAction> actOffBall(double time) override;
		void matchHalfChanged(MatchHalf m) override;

	private:
		void setPivotPoint();
		boost::shared_ptr<PlayerAction> jumpToBall(double time);

		Common::Vector3 mPivotPoint;
		float mDistanceFromPivot;
		Countdown mHoldBallTimer;
};

class AIDefendState : public AIState {
	public:
		AIDefendState(Player* p, AIPlayController* m);
		boost::shared_ptr<PlayerAction> actOffBall(double time) override;
};

class AIKickBallState : public AIState {
	public:
		AIKickBallState(Player* p, AIPlayController* m);
		boost::shared_ptr<PlayerAction> actOnBall(double time) override;
		boost::shared_ptr<PlayerAction> actNearBall(double time) override;
		boost::shared_ptr<PlayerAction> actOffBall(double time) override;
};

class AIOffensiveState : public AIState {
	public:
		AIOffensiveState(Player* p, AIPlayController* m);
		boost::shared_ptr<PlayerAction> actOffBall(double time) override;
};

class AIMidfielderState : public AIState {
	public:
		AIMidfielderState(Player* p, AIPlayController* m);
		boost::shared_ptr<PlayerAction> actOffBall(double time) override;
};



#endif

