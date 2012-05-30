#ifndef SOCCER_TEAM_H
#define SOCCER_TEAM_H

#include <vector>
#include <memory>

#include "common/Color.h"

#include "soccer/DataExchange.h"
#include "soccer/Player.h"
#include "soccer/Continent.h"
#include "soccer/PlayerTactics.h"

namespace Soccer {

class Team;

class TeamTactics {
	public:
		TeamTactics(const Team& team);
		/* NOTE: the key is the player index (1..11). */
		std::map<int, PlayerTactics> mTactics;
		float Pressure;    // counter attacks vs. pressure
		float Organized;   // creative vs. organized
		float LongBalls;   // short passes vs. long balls
		float AttackWings; // attacks on center vs .wings
};

struct TeamController {
	TeamController(bool h, int sp)
		: HumanControlled(h),
		PlayerShirtNumber(sp) { }
	bool HumanControlled;
	int PlayerShirtNumber;
};

class Kit {
	public:
		enum class KitType {
			Plain,
			Striped,
			HorizontalStriped,
			ColoredSleeves
		};

		Kit(KitType t, const Common::Color& shirt1, const Common::Color& shirt2,
				const Common::Color& shorts, const Common::Color& socks);
		const Common::Color& getPrimaryShirtColor() const;
		const Common::Color& getSecondaryShirtColor() const;
		const Common::Color& getShortsColor() const;
		const Common::Color& getSocksColor() const;
		KitType getKitType() const;

	private:
		KitType mType;
		Common::Color mPrimaryShirtColor;
		Common::Color mSecondaryShirtColor;
		Common::Color mShortsColor;
		Common::Color mSocksColor;
};

class Team {
	public:
		Team(int id, const char* name, const Kit& homekit, const Kit& awaykit, const std::vector<int>& players);
		Team(int id, const char* name, const Kit& homekit, const Kit& awaykit, const std::vector<std::shared_ptr<Player>>& players);
		virtual ~Team() { }
		void addPlayer(std::shared_ptr<Player> p);
		const std::shared_ptr<Player> getPlayer(unsigned int i) const;
		void fetchPlayersFromDB(const PlayerDatabase& db);
		int getId() const;
		const std::string& getName() const;
		const std::vector<std::shared_ptr<Player>>& getPlayers() const;
		const std::shared_ptr<Player> getPlayerById(int i) const;
		const Kit& getHomeKit() const;
		const Kit& getAwayKit() const;
	protected:
		int mId;
		std::string mName;
	private:
		std::vector<int> mPlayerIds;
		std::vector<std::shared_ptr<Player>> mPlayers;
		Kit mHomeKit;
		Kit mAwayKit;
};

class StatefulTeam : public Team {
	public:
		StatefulTeam(const Team& t, TeamController c, const TeamTactics& tt);
		const TeamController& getController() const;
		const TeamTactics& getTactics() const;

	private:
		TeamController mController;
	protected:
		TeamTactics mTactics;
};


}


#endif

