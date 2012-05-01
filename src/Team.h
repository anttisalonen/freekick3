#ifndef TEAM_H
#define TEAM_H

#include <vector>

#include "Player.h"
#include "Distance.h"

class Team {
	public:
		Team(Match* match, bool first);
		void addPlayer(std::shared_ptr<Player> p);
		Player* getPlayer(unsigned int idx);
		const Player* getPlayer(unsigned int idx) const;
		unsigned int getNumPlayers() const;
		const std::vector<std::shared_ptr<Player>>& getPlayers() const;
		RelVector3 getPausePosition() const;
		bool isFirst() const;
		void act(double time);
		const Match* getMatch() const;

		Player* getPlayerNearestToBall() const;
	private:
		void updatePlayerNearestToBall();
		Match* mMatch;
		bool mFirst;
		std::vector<std::shared_ptr<Player>> mPlayers;
		Player* mPlayerNearestToBall;
};

#endif
