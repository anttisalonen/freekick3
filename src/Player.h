#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "Distance.h"

class Match;
class Team;
class PlayerAction;
class PlayerController;

class Player {
	public:
		Player(Match* match, Team* team);
		std::shared_ptr<PlayerAction> act();
		const Match* getMatch() const;
		const Team* getTeam() const;
		const RelVector3& getHomePosition() const;
		void setHomePosition(const RelVector3& p);
		float getRunSpeed() const; // m/s
		void move(const AbsVector3& v);
		const AbsVector3& getPosition() const;
	private:
		Match* mMatch;
		Team* mTeam;
		std::shared_ptr<PlayerController> mController;
		RelVector3 mHomePosition;
		AbsVector3 mPosition;
};

#endif


