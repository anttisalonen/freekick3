#ifndef SOCCER_TEAM_H
#define SOCCER_TEAM_H

#include <vector>
#include <memory>

#include "soccer/Player.h"

namespace Soccer {

class Team {
	public:
		Team();
		void addPlayer(std::shared_ptr<Player> p);
		const std::shared_ptr<Player> getPlayer(unsigned int i) const;
	private:
		std::vector<std::shared_ptr<Player>> mPlayers;
};


}


#endif

