#ifndef SOCCER_TEAM_H
#define SOCCER_TEAM_H

#include <vector>
#include <memory>

#include "soccer/DataExchange.h"
#include "soccer/Player.h"
#include "soccer/Continent.h"
#include "soccer/PlayerTactics.h"

namespace Soccer {

class TeamTactics {
	public:
		TeamTactics();
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

class Team {
	public:
		Team(int id, const char* name, const std::vector<int>& players);
		Team(int id, const char* name, const std::vector<std::shared_ptr<Player>>& players);
		virtual ~Team() { }
		void addPlayer(std::shared_ptr<Player> p);
		const std::shared_ptr<Player> getPlayer(unsigned int i) const;
		void fetchPlayersFromDB(const PlayerDatabase& db);
		int getId() const;
		const std::string& getName() const;
	protected:
		int mId;
		std::string mName;
	private:
		std::vector<int> mPlayerIds;
		std::vector<std::shared_ptr<Player>> mPlayers;
};

class StatefulTeam : public Team {
	public:
		StatefulTeam(const Team& t, TeamController c, const TeamTactics& tt);
		const TeamController& getController() const;

	private:
		TeamController mController;
	protected:
		TeamTactics mTactics;
};


}


#endif

