#ifndef SOCCER_CONTINENT_H
#define SOCCER_CONTINENT_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "soccer/Container.h"
#include "soccer/Tournament.h"
#include "soccer/Team.h"


namespace Soccer {

class Team;
class Player;

class League : public Container<Team> {
	public:
		League(const char* name, unsigned int level)
			: Container<Team>(name),
			mLevel(level) { }
		unsigned int getLevel() const { return mLevel; }
		std::vector<boost::shared_ptr<Team>> getTeamsByPosition() const;
		unsigned int getNumberOfTeams() const;

	private:
		unsigned int mLevel;
};

class LeagueSystem : public Container<League> {
	public:
		LeagueSystem(const char* name)
			: Container<League>(name) { }
		std::vector<boost::shared_ptr<League>> getLeagues() const
		{
			std::vector<boost::shared_ptr<League>> ret;
			for(auto& t : getContainer())
				ret.push_back(t.second);
			return ret;
		}
};

class Continent : public Container<LeagueSystem> {
	public:
		Continent(const char* name)
			: Container<LeagueSystem>(name) { }
		const std::vector<TournamentConfig>& getTournaments() const { return mTournaments; }
		void addTournament(const TournamentConfig& t) { mTournaments.push_back(t); }

	private:
		std::vector<TournamentConfig> mTournaments;
};

class TeamDatabase : public Container<Continent> {
	public:
		TeamDatabase()
			: Container<Continent>("Team database") { }
		boost::shared_ptr<Continent> getOrCreateContinent(const char* n);
		boost::shared_ptr<LeagueSystem> getOrCreateLeagueSystem(const char* continentName,
				const char* countryName);
		// level only used when creating league
		boost::shared_ptr<League> getOrCreateLeague(const char* continentName,
				const char* countryName, const char* leagueName, unsigned int level);
};

}

#endif



