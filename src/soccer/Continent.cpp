#include "soccer/Continent.h"

namespace Soccer {

std::vector<boost::shared_ptr<Team>> League::getTeamsByPosition() const
{
	std::vector<boost::shared_ptr<Team>> ret;
	for(auto t : mTs) {
		ret.push_back(t.second);
	}
	std::sort(ret.begin(), ret.end(), [](boost::shared_ptr<Team> t1, boost::shared_ptr<Team> t2) {
			return t1->getPosition() < t2->getPosition(); });
	return ret;
}

unsigned int League::getNumberOfTeams() const
{
	return mTs.size();
}

boost::shared_ptr<Continent> TeamDatabase::getOrCreateContinent(const char* n)
{
	boost::shared_ptr<Soccer::Continent> cont = getT(n);
	if(!cont) {
		cont = boost::shared_ptr<Soccer::Continent>(new Soccer::Continent(n));
		addT(cont);
	}
	return cont;
}

boost::shared_ptr<LeagueSystem> TeamDatabase::getOrCreateLeagueSystem(const char* continentName,
		const char* countryName)
{
	auto cont = getOrCreateContinent(continentName);
	auto lsys = cont->getT(countryName);
	if(!lsys) {
		lsys = boost::shared_ptr<Soccer::LeagueSystem>(new Soccer::LeagueSystem(countryName));
		cont->addT(lsys);
	}
	return lsys;
}

boost::shared_ptr<League> TeamDatabase::getOrCreateLeague(const char* continentName,
		const char* countryName, const char* leagueName, unsigned int level)
{
	auto lsys = getOrCreateLeagueSystem(continentName, countryName);
	auto leag = lsys->getT(leagueName);
	if(!leag) {
		leag = boost::shared_ptr<Soccer::League>(new Soccer::League(leagueName, level));
		lsys->addT(leag);
	}
	return leag;
}

}

