#ifndef SOCCER_TEAM_H
#define SOCCER_TEAM_H

#include <vector>
#include <memory>

#include "soccer/DataExchange.h"
#include "soccer/Player.h"

namespace Soccer {

class Team {
	public:
		Team(int id, const char* name, const std::vector<int>& players);
		Team(int id, const char* name, const std::vector<std::shared_ptr<Player>>& players);
		void addPlayer(std::shared_ptr<Player> p);
		const std::shared_ptr<Player> getPlayer(unsigned int i) const;
		void fetchPlayersFromDB(const PlayerDatabase& db);
		int getId() const;
		const std::string& getName() const;
	private:
		int mId;
		std::string mName;
		std::vector<int> mPlayerIds;
		std::vector<std::shared_ptr<Player>> mPlayers;
};


}


#endif

