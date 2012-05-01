#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "MatchEntity.h"
#include "Distance.h"
#include "Clock.h"
#include "PlayerTactics.h"

class Match;
class Team;
class PlayerAction;
class PlayerController;
class PlayerAIController;

class Player : public MatchEntity {
	public:
		Player(Match* match, Team* team, bool goalkeeper);
		~Player();
		std::shared_ptr<PlayerAction> act(double time);
		const Team* getTeam() const;
		const RelVector3& getHomePosition() const;
		void setHomePosition(const RelVector3& p);
		double getRunSpeed() const; // m/s
		double getMaximumKickPower() const; // ball speed in m/s
		void setController(PlayerController* c);
		void setAIControlled();
		bool isAIControlled() const;
		bool isGoalkeeper() const;
		void ballKicked();
		bool canKickBall() const;
		void update(float time) override;
		void setPlayerTactics(const PlayerTactics& t);
		const PlayerTactics& getTactics() const;
	private:
		Team* mTeam;
		PlayerController* mController;
		PlayerAIController* mAIController;
		RelVector3 mHomePosition;
		bool mGoalkeeper;
		Countdown mBallKickedTimer;
		PlayerTactics mTactics;
};

#endif


