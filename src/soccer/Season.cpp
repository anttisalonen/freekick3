#include "Season.h"

namespace Soccer {

StatefulLeagueSystem::StatefulLeagueSystem()
{
}

void StatefulLeagueSystem::addLeague(boost::shared_ptr<StatefulLeague> l)
{
	mLeagues.push_back(l);
}

void StatefulLeagueSystem::setCup(boost::shared_ptr<StatefulCup> c)
{
	mCup = c;
}

std::vector<boost::shared_ptr<StatefulLeague>>& StatefulLeagueSystem::getLeagues()
{
	return mLeagues;
}

void StatefulLeagueSystem::promoteAndRelegateTeams()
{
	std::vector<std::vector<boost::shared_ptr<StatefulTeam>>> oldLeagueTeams;
	std::vector<std::vector<boost::shared_ptr<StatefulTeam>>> newLeagueTeams;

	for(auto it = mLeagues.begin(); it != mLeagues.end(); ++it) {
		oldLeagueTeams.push_back((*it)->getTeamsByPosition());
	}

	for(auto it = oldLeagueTeams.begin(); it != oldLeagueTeams.end(); ++it) {
		/* Relegate three teams from this league, promote three teams from the next league */
		auto thisLeague = *it;
		++it;
		if(it == oldLeagueTeams.end())
			break;
		auto nextLeague = *it;

		std::vector<boost::shared_ptr<StatefulTeam>> newThisLeague;
		std::vector<boost::shared_ptr<StatefulTeam>> newNextLeague;
		unsigned int teamsToMove = 3;
		if(thisLeague.size() / 2 < teamsToMove)
			teamsToMove = thisLeague.size() / 2;
		if(nextLeague.size() / 2 < teamsToMove)
			teamsToMove = nextLeague.size() / 2;

		for(unsigned int i = 0; i < teamsToMove; i++) {
			newNextLeague.push_back(thisLeague.back());
			thisLeague.pop_back();
			newThisLeague.push_back(nextLeague.front());
			nextLeague.erase(nextLeague.begin());
		}

		for(auto t : thisLeague) {
			newThisLeague.push_back(t);
		}

		for(auto t : nextLeague) {
			newNextLeague.push_back(t);
		}

		newLeagueTeams.push_back(newThisLeague);
		newLeagueTeams.push_back(newNextLeague);
	}

	assert(mLeagues.size() == newLeagueTeams.size());

	for(unsigned int i = 0; i < mLeagues.size(); i++) {
		mLeagues[i] = boost::shared_ptr<StatefulLeague>(new StatefulLeague(newLeagueTeams[i]));
	}
}


Season::Season(boost::shared_ptr<StatefulTeam> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c,
		boost::shared_ptr<StatefulLeagueSystem> ls)
{
	reset(t, l, c, ls);
}

void Season::reset(boost::shared_ptr<StatefulTeam> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c,
		boost::shared_ptr<StatefulLeagueSystem> ls)
{
	mTeam = t;
	mLeague = l;
	mCup = c;
	mLeagueSystem = ls;
	createSchedule();
}

Season::Season()
{
}

boost::shared_ptr<StatefulTeam> Season::getTeam()
{
	return mTeam;
}

boost::shared_ptr<StatefulLeague> Season::getLeague()
{
	return mLeague;
}

boost::shared_ptr<StatefulCup> Season::getCup()
{
	return mCup;
}

boost::shared_ptr<StatefulTournament> Season::getTournament()
{
	return mTournament;
}

boost::shared_ptr<StatefulLeagueSystem> Season::getLeagueSystem()
{
	return mLeagueSystem;
}

void Season::createSchedule()
{
	mSchedule.clear();

	unsigned int cupRounds = mCup->getTotalNumberOfRounds();
	unsigned int leagueRounds = mLeague->getSchedule().getNumberOfRounds();
	unsigned int totalCupRounds = cupRounds;
	unsigned int totalLeagueRounds = leagueRounds;

	if(!leagueRounds) {
		for(unsigned int i = 0; i < cupRounds; i++)
			mSchedule.push_back({CompetitionType::Cup, i});

		return;
	}

	if(!cupRounds) {
		for(unsigned int i = 0; i < leagueRounds; i++)
			mSchedule.push_back({CompetitionType::League, i});

		return;
	}

	float ratio = leagueRounds/(float)cupRounds;
	float counter = 0.0f;

	while(cupRounds || leagueRounds) {
		counter += ratio;
		while(counter > 0.0f) {
			if(leagueRounds) {
				mSchedule.push_back({CompetitionType::League, totalLeagueRounds - leagueRounds});
				leagueRounds--;
			}
			counter -= 1.0f;
		}
		if(cupRounds) {
			mSchedule.push_back({CompetitionType::Cup, totalCupRounds - cupRounds});
			cupRounds--;
		}
	}
}

const std::vector<std::pair<CompetitionType, unsigned int>>& Season::getSchedule() const
{
	return mSchedule;
}

boost::shared_ptr<Season> Season::createSeason(boost::shared_ptr<StatefulTeam> team,
		boost::shared_ptr<StatefulLeagueSystem> leaguesystem)
{
	boost::shared_ptr<StatefulCup> cup;
	boost::shared_ptr<StatefulLeague> plleague;

	for(auto& l : leaguesystem->getLeagues()) {
		bool playerLeagueFound = false;

		for(auto& t : l->getTeamsByPosition()) {
			if(t == team) {
				playerLeagueFound = true;
			}
		}

		if(playerLeagueFound) {
			assert(!plleague);
			plleague = l;
		}
	}
	assert(plleague);

	{
		std::vector<boost::shared_ptr<StatefulTeam>> cupteams = StatefulCup::collectTeamsFromCountry(leaguesystem);
		cup = boost::shared_ptr<StatefulCup>(new StatefulCup(cupteams));
		leaguesystem->setCup(cup);
	}

	assert(team);

	boost::shared_ptr<Season> season(new Season(team, plleague, cup, leaguesystem));
	return season;
}

boost::shared_ptr<Season> Season::createSeason(boost::shared_ptr<Team> plteam,
		boost::shared_ptr<LeagueSystem> country, bool addSystem)
{
	std::map<boost::shared_ptr<Team>, boost::shared_ptr<StatefulTeam>> allteams;

	boost::shared_ptr<StatefulLeagueSystem> leaguesystem;
	if(addSystem)
		leaguesystem = boost::shared_ptr<StatefulLeagueSystem>(new StatefulLeagueSystem());
	boost::shared_ptr<StatefulCup> cup;
	boost::shared_ptr<StatefulLeague> plleague;
	boost::shared_ptr<StatefulTeam> team;

	std::map<unsigned int, boost::shared_ptr<StatefulLeague>> leagues;

	for(auto& l : country->getContainer()) {
		bool playerLeagueFound = false;
		assert(leagues.find(l.second->getLevel()) == leagues.end());

		if(l.second->getName() == "Non-League")
			continue;

		std::vector<boost::shared_ptr<StatefulTeam>> leagueteams;
		for(auto& t : l.second->getContainer()) {
			boost::shared_ptr<StatefulTeam> st(new StatefulTeam(*t.second,
						TeamController(t.second == plteam, 0),
						AITactics::createTeamTactics(*t.second)));
			allteams.insert({t.second, st});
			if(t.second == plteam) {
				playerLeagueFound = true;
				team = st;
			}
			leagueteams.push_back(st);
		}
		boost::shared_ptr<StatefulLeague> league = boost::shared_ptr<StatefulLeague>(new StatefulLeague(leagueteams));
		leagues.insert({l.second->getLevel(), league});

		if(playerLeagueFound) {
			assert(!plleague);
			plleague = league;
		}
	}
	assert(plleague);

	if(addSystem) {
		for(auto& l : leagues) {
			leaguesystem->addLeague(l.second);
		}
	}

	{
		std::vector<boost::shared_ptr<StatefulTeam>> cupteams;
		std::vector<boost::shared_ptr<Team>> cupteamentries;
		cupteamentries = StatefulCup::collectTeamsFromCountry(country);
		for(auto t : cupteamentries) {
			auto it = allteams.find(t);
			if(it == allteams.end()) {
				cupteams.push_back(boost::shared_ptr<StatefulTeam>(new StatefulTeam(*t,
								TeamController(t == plteam, 0),
								AITactics::createTeamTactics(*t))));
			} else {
				cupteams.push_back(it->second);
			}
		}
		cup = boost::shared_ptr<StatefulCup>(new StatefulCup(cupteams));
		if(addSystem) {
			leaguesystem->setCup(cup);
		}
	}

	assert(team);

	boost::shared_ptr<Season> season(new Season(team, plleague, cup, leaguesystem));
	return season;
}

}

