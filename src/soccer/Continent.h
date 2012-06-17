#ifndef SOCCER_CONTINENT_H
#define SOCCER_CONTINENT_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

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
		boost::shared_ptr<Continent> getOrCreateContinent(const char* n);
		boost::shared_ptr<LeagueSystem> getOrCreateLeagueSystem(const char* continentName,
				const char* countryName);
		boost::shared_ptr<League> getOrCreateLeague(const char* continentName,
				const char* countryName, const char* leagueName);
};

typedef std::map<int, boost::shared_ptr<Player>> PlayerDatabase;

}

#endif



