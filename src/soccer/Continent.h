#ifndef SOCCER_CONTINENT_H
#define SOCCER_CONTINENT_H

#include <string>
#include <vector>
#include <memory>

#include "soccer/Container.h"


namespace Soccer {

class Team;
class Player;

class League : public Container<Team> {
	public:
		League(const char* name)
			: Container<Team>(name) { }
};

class LeagueSystem : public Container<League> {
	public:
		LeagueSystem(const char* name)
			: Container<League>(name) { }
};

class Continent : public Container<LeagueSystem> {
	public:
		Continent(const char* name)
			: Container<LeagueSystem>(name) { }
};

class TeamDatabase : public Container<Continent> {
	public:
		TeamDatabase()
			: Container<Continent>("Team database") { }
		std::shared_ptr<Continent> getOrCreateContinent(const char* n);
		std::shared_ptr<LeagueSystem> getOrCreateLeagueSystem(const char* continentName,
				const char* countryName);
		std::shared_ptr<League> getOrCreateLeague(const char* continentName,
				const char* countryName, const char* leagueName);
};

typedef std::map<int, std::shared_ptr<Player>> PlayerDatabase;

}

#endif



