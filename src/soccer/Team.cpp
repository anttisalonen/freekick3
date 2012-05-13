#include <sstream>
#include <stdexcept>

#include "soccer/Team.h"

namespace Soccer {

Team::Team(int id, const char* name, const std::vector<int>& players)
	: mId(id),
	mName(name),
	mPlayerIds(players)
{
}

Team::Team(int id, const char* name, const std::vector<std::shared_ptr<Player>>& players)
	: mId(id),
	mName(name),
	mPlayers(players)
{
}

void Team::addPlayer(std::shared_ptr<Player> p)
{
	mPlayers.push_back(p);
}

const std::shared_ptr<Player> Team::getPlayer(unsigned int i) const
{
	if(i >= mPlayers.size())
		return nullptr;
	else
		return mPlayers[i];
}

void Team::fetchPlayersFromDB(const PlayerDatabase& db)
{
	for(auto id : mPlayerIds) {
		auto it = db.find(id);
		if(it == db.end()) {
			std::stringstream ss;
			ss << "Player " << id << " not found in the player database!";
			throw std::runtime_error(ss.str());
		}
		mPlayers.push_back(it->second);
	}
	mPlayerIds.clear();
}

int Team::getId() const
{
	return mId;
}

const std::string& Team::getName() const
{
	return mName;
}


}
