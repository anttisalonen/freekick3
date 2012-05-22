#include <sstream>
#include <stdexcept>

#include "soccer/Team.h"

namespace Soccer {

TeamTactics::TeamTactics()
{
	// 4-4-2.
	for(int i = 0; i < 11; i++) {
		PlayerTactics t(0.5, 0.40f);
		switch(i) {
			case 0:
			default:
				break;
			case 1:
				t.WidthPosition = -0.50;
				t.Radius = 0.20f;
				break;
			case 2:
				t.WidthPosition = -0.20;
				t.Radius = 0.40f;
				break;
			case 3:
				t.WidthPosition = 0.20;
				t.Radius = 0.40f;
				break;
			case 4:
				t.WidthPosition = 0.50;
				t.Radius = 0.20f;
				break;
			case 5:
				t.WidthPosition = -0.70;
				t.Radius = 0.30f;
				break;
			case 6:
				t.WidthPosition = -0.20;
				break;
			case 7:
				t.WidthPosition = 0.20;
				break;
			case 8:
				t.WidthPosition = 0.70;
				t.Radius = 0.30f;
				break;
			case 9:
				t.WidthPosition = -0.30;
				break;
			case 10:
				t.WidthPosition = 0.30;
				break;
		}
		mTactics.insert(std::make_pair(i + 1, t));
	}
}

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


StatefulTeam::StatefulTeam(const Team& t, TeamController c, const TeamTactics& tt)
	: Team(t),
	mController(c),
	mTactics(tt)
{
}

const TeamController& StatefulTeam::getController() const
{
	return mController;
}

}
