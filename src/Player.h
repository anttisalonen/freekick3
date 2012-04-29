#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "MatchEntity.h"
#include "Distance.h"

class Match;
class Team;
class PlayerAction;
class PlayerController;
class PlayerAIController;

class Player : public MatchEntity {
	public:
		Player(Match* match, Team* team);
		~Player();
		std::shared_ptr<PlayerAction> act();
		const Team* getTeam() const;
		const RelVector3& getHomePosition() const;
		void setHomePosition(const RelVector3& p);
		float getRunSpeed() const; // m/s
		void setController(PlayerController* c);
		void setAIControlled();
	private:
		Team* mTeam;
		PlayerController* mController;
		PlayerAIController* mAIController;
		RelVector3 mHomePosition;
};

#endif


