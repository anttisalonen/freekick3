#include "Team.h"

Team::Team(Match* match, bool first)
	: mMatch(match),
	mFirst(first)
{
}

void Team::addPlayer(std::shared_ptr<Player> p)
{
	mPlayers.push_back(p);
	if(mPlayers.size() == 1) {
		p->setHomePosition(RelVector3(0, -0.95f * (mFirst ? 1 : -1), 0));
	}
	else {
		int hgt = (mPlayers.size() - 2) / 4;
		int widx = (mPlayers.size() - 2) % 4;
		if(hgt >= 2) {
			if(mFirst) {
				p->setHomePosition(RelVector3(widx < 1 ? -0.1f : 0.1f,
							-0.01f, 0));
			}
			else {
				p->setHomePosition(RelVector3(widx < 1 ? -0.1f : 0.1f,
							0.15f, 0));
			}
		}
		else {
			float wdt = (widx - 1.5f) * 0.5f;
			p->setHomePosition(RelVector3(wdt, (mFirst ? 1 : -1) * -0.7f + hgt * 0.3f * (mFirst ? 1 : -1), 0));
		}
	}
}

Player* Team::getPlayer(unsigned int idx)
{
	if(idx >= mPlayers.size())
		return nullptr;
	else
		return mPlayers[idx].get();
}

const Player* Team::getPlayer(unsigned int idx) const
{
	if(idx >= mPlayers.size())
		return nullptr;
	else
		return mPlayers[idx].get();
}

unsigned int Team::getNumPlayers() const
{
	return mPlayers.size();
}

const std::vector<std::shared_ptr<Player>>& Team::getPlayers() const
{
	return mPlayers;
}

RelVector3 Team::getPausePosition() const
{
	return RelVector3(1.2f, 0.0f, 0.0f);
}

bool Team::isFirst() const
{
	return mFirst;
}


