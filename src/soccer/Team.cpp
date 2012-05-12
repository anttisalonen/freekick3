#include "soccer/Team.h"

namespace Soccer {

Team::Team()
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


}
