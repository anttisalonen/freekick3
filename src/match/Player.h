#ifndef PLAYER_H
#define PLAYER_H

#include <boost/shared_ptr.hpp>

#include "soccer/Player.h"
#include "soccer/PlayerTactics.h"

#include "match/MatchEntity.h"
#include "match/Distance.h"
#include "match/Clock.h"

class Match;
class Team;
class PlayerAction;
class PlayerController;
class PlayerAIController;

enum class MatchHalf;

typedef int ShirtNumber;

class Player : public MatchEntity, public Soccer::Player {
	public:
		Player(Match* match, Team* team, const Soccer::Player& p,
				ShirtNumber sn, const PlayerTactics& t);
		~Player();
		boost::shared_ptr<PlayerAction> act(double time);
		int getShirtNumber() const;
		const Team* getTeam() const;
		Team* getTeam();
		const RelVector3& getHomePosition() const;
		void setHomePosition(const RelVector3& p);
		double getRunSpeed() const; // m/s
		double getMaximumShotPower() const; // ball speed in m/s
		void setController(PlayerController* c);
		void setAIControlled();
		bool isAIControlled() const;
		void ballKicked();
		bool canKickBall() const;
		void update(float time) override;
		void setPlayerTactics(const PlayerTactics& t);
		const PlayerTactics& getTactics() const;
		const PlayerAIController* getAIController() const;
		void matchHalfChanged(MatchHalf m);
		void setTackling();
		void setTackled();
		bool standing() const;
		bool tackling() const;
		bool isAirborne() const;
	private:
		Team* mTeam;
		PlayerController* mController;
		PlayerAIController* mAIController;
		RelVector3 mHomePosition;
		Countdown mBallKickedTimer;
		PlayerTactics mTactics;
		ShirtNumber mShirtNumber;
		Countdown mTacklingTimer;
		Countdown mTackledTimer;
};

#endif


