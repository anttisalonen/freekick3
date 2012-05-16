#include "soccer/Continent.h"

namespace Soccer {

std::shared_ptr<Continent> TeamDatabase::getOrCreateContinent(const char* n)
{
	std::shared_ptr<Soccer::Continent> cont = getT(n);
	if(!cont) {
		cont = std::shared_ptr<Soccer::Continent>(new Soccer::Continent(n));
		addT(cont);
	}
	return cont;
}

std::shared_ptr<LeagueSystem> TeamDatabase::getOrCreateLeagueSystem(const char* continentName,
		const char* countryName)
{
	auto cont = getOrCreateContinent(continentName);
	auto lsys = cont->getT(countryName);
	if(!lsys) {
		lsys = std::shared_ptr<Soccer::LeagueSystem>(new Soccer::LeagueSystem(countryName));
		cont->addT(lsys);
	}
	return lsys;
}

std::shared_ptr<League> TeamDatabase::getOrCreateLeague(const char* continentName,
		const char* countryName, const char* leagueName)
{
	auto lsys = getOrCreateLeagueSystem(continentName, countryName);
	auto leag = lsys->getT(leagueName);
	if(!leag) {
		leag = std::shared_ptr<Soccer::League>(new Soccer::League(leagueName));
		lsys->addT(leag);
	}
	return leag;
}

}

