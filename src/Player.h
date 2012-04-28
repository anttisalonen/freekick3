#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "MatchEntity.h"
#include "Distance.h"

class Match;
class Team;
class PlayerAction;
class PlayerController;

class Player : public MatchEntity {
	public:
		Player(Match* match, Team* team);
		std::shared_ptr<PlayerAction> act();
		const Team* getTeam() const;
		const RelVector3& getHomePosition() const;
		void setHomePosition(const RelVector3& p);
		float getRunSpeed() const; // m/s
	private:
		Team* mTeam;
		std::shared_ptr<PlayerController> mController;
		RelVector3 mHomePosition;
};

#endif


