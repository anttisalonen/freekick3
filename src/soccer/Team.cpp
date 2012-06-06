#include <sstream>
#include <stdexcept>

#include "soccer/Team.h"

namespace Soccer {

using namespace Common;

TeamTactics::TeamTactics(const Team& team)
	: Pressure(0.5f),
	Organized(0.5f),
	LongBalls(0.5f),
	AttackWings(0.5f)
{
	// 4-4-2.
	int gk = 0;
	int df = 0;
	int mf = 0;
	int fw = 0;
	for(auto p : team.getPlayers()) {
		PlayerTactics t(0.5, 0.40f);
		switch(p->getPlayerPosition()) {
			case PlayerPosition::Goalkeeper:
				if(gk < 1) {
					mTactics.insert(std::make_pair(p->getId(), t));
					gk++;
				}
				break;

			case PlayerPosition::Defender:
				if(df < 4) {
					if(df == 0) {
						t.WidthPosition = -0.50;
						t.Radius = 0.55f;
					}
					else if(df == 1) {
						t.WidthPosition = -0.20;
						t.Radius = 0.40f;
					}
					else if(df == 2) {
						t.WidthPosition = 0.20;
						t.Radius = 0.40f;
					}
					else {
						t.WidthPosition = 0.50;
						t.Radius = 0.55f;
					}
					mTactics.insert(std::make_pair(p->getId(), t));
					df++;
				}
				break;

			case PlayerPosition::Midfielder:
				if(mf < 4) {
					if(mf == 0) {
						t.WidthPosition = -0.70;
						t.Radius = 0.30f;
					}
					else if(mf == 1) {
						t.WidthPosition = -0.20;
					}
					else if(mf == 2) {
						t.WidthPosition = 0.20;
					}
					else {
						t.WidthPosition = 0.70;
						t.Radius = 0.30f;
					}
					mTactics.insert(std::make_pair(p->getId(), t));
					mf++;
				}
				break;

			case PlayerPosition::Forward:
				if(fw < 2) {
					if(fw == 0) {
						t.WidthPosition = -0.30;
					}
					else {
						t.WidthPosition = 0.30;
					}
					mTactics.insert(std::make_pair(p->getId(), t));
					fw++;
				}
				break;
		}
		if(gk + df + mf + fw >= 11)
			break;
	}
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

Team::Team(int id, const char* name, const Kit& homekit, const Kit& awaykit, const std::vector<std::shared_ptr<Player>>& players)
	: mId(id),
	mName(name),
	mPlayers(players),
	mHomeKit(homekit),
	mAwayKit(awaykit)
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

const std::vector<std::shared_ptr<Player>>& Team::getPlayers() const
{
	return mPlayers;
}

const std::shared_ptr<Player> Team::getPlayerById(int i) const
{
	for(auto p : mPlayers)
		if(p->getId() == i)
			return p;
	return nullptr;
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

const TeamTactics& StatefulTeam::getTactics() const
{
	return mTactics;
}

}
