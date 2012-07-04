#include <sstream>
#include <stdexcept>

#include "soccer/Team.h"

namespace Soccer {

using namespace Common;

TeamTactics::TeamTactics(const std::map<int, PlayerTactics>& pt,
		float pressure, float longballs,
		float fastpassing, float shootclose)
	: mTactics(pt),
	Pressure(pressure),
	LongBalls(longballs),
	FastPassing(fastpassing),
	ShootClose(shootclose)
{
}

Kit::Kit(KitType t, const Common::Color& shirt1, const Common::Color& shirt2,
		const Common::Color& shorts, const Common::Color& socks)
	: mType(t),
	mPrimaryShirtColor(shirt1),
	mSecondaryShirtColor(shirt2),
	mShortsColor(shorts),
	mSocksColor(socks)
{
	if(mType == KitType::Plain)
		mSecondaryShirtColor = mPrimaryShirtColor;
}

Kit::KitType Kit::getKitType() const
{
	return mType;
}

const Color& Kit::getPrimaryShirtColor() const
{
	return mPrimaryShirtColor;
}

const Color& Kit::getSecondaryShirtColor() const
{
	return mSecondaryShirtColor;
}

const Color& Kit::getShortsColor() const
{
	return mShortsColor;
}

const Color& Kit::getSocksColor() const
{
	return mSocksColor;
}


Team::Team(int id, const char* name, const Kit& homekit, const Kit& awaykit, const std::vector<int>& players)
	: mId(id),
	mName(name),
	mPlayerIds(players),
	mHomeKit(homekit),
	mAwayKit(awaykit)
{
}

Team::Team(int id, const char* name, const Kit& homekit, const Kit& awaykit, const std::vector<boost::shared_ptr<Player>>& players)
	: mId(id),
	mName(name),
	mPlayers(players),
	mHomeKit(homekit),
	mAwayKit(awaykit)
{
}

void Team::addPlayer(boost::shared_ptr<Player> p)
{
	mPlayers.push_back(p);
}

const boost::shared_ptr<Player> Team::getPlayer(unsigned int i) const
{
	if(i >= mPlayers.size())
		return boost::shared_ptr<Player>();
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

const std::vector<boost::shared_ptr<Player>>& Team::getPlayers() const
{
	return mPlayers;
}

const boost::shared_ptr<Player> Team::getPlayerById(int i) const
{
	for(auto p : mPlayers)
		if(p->getId() == i)
			return p;
	return boost::shared_ptr<Player>();
}

const Kit& Team::getHomeKit() const
{
	return mHomeKit;
}

const Kit& Team::getAwayKit() const
{
	return mAwayKit;
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

TeamController& StatefulTeam::getController()
{
	return mController;
}

const TeamTactics& StatefulTeam::getTactics() const
{
	return mTactics;
}

void StatefulTeam::setTactics(const TeamTactics& t)
{
	mTactics = t;
}

StatefulTeam::StatefulTeam()
{
}

TeamTactics::TeamTactics()
{
}

TeamController::TeamController()
{
}

Kit::Kit()
{
}

Team::Team()
{
}

}
