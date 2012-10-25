#include <algorithm>
#include "League.h"

namespace Soccer {

StatefulLeague::StatefulLeague(std::vector<boost::shared_ptr<StatefulTeam>>& teams, unsigned int numCycles)
	: StatefulCompetition(),
	mPointsPerWin(3),
	mNumCycles(numCycles)
{
	setRoundRobin(teams);
	setNextMatch();
}

void StatefulLeague::resetTeams(std::vector<boost::shared_ptr<StatefulTeam>>& teams)
{
	setRoundRobin(teams);
	setNextMatch();
}

void StatefulLeague::matchPlayed(const MatchResult& res)
{
	assert(mNextMatch);
	assert(res.Played);

	auto meit1 = mEntries.find(mNextMatch->getTeam(0));
	auto meit2 = mEntries.find(mNextMatch->getTeam(1));
	assert(meit1 != mEntries.end());
	assert(meit2 != mEntries.end());

	meit1->second.GoalsFor     += res.HomeGoals;
	meit1->second.GoalsAgainst += res.AwayGoals;
	meit2->second.GoalsFor     += res.AwayGoals;
	meit2->second.GoalsAgainst += res.HomeGoals;

	meit1->second.Matches++;
	meit2->second.Matches++;
	if(res.HomeGoals > res.AwayGoals) {
		meit1->second.Points += mPointsPerWin;
		meit1->second.Wins++;
		meit2->second.Losses++;
	}
	else if(res.AwayGoals > res.HomeGoals) {
		meit2->second.Points += mPointsPerWin;
		meit2->second.Wins++;
		meit1->second.Losses++;
	}
	else {
		meit1->second.Points++;
		meit2->second.Points++;
		meit1->second.Draws++;
		meit2->second.Draws++;
	}
	setNextMatch();
}

const std::map<boost::shared_ptr<StatefulTeam>, LeagueEntry>& StatefulLeague::getEntries() const
{
	return mEntries;
}

void StatefulLeague::setRoundRobin(std::vector<boost::shared_ptr<StatefulTeam>>& teams)
{
	mEntries.clear();
	for(auto t : teams) {
		mEntries.insert(std::make_pair(t, LeagueEntry()));
	}

	int odd = 0;
	if((teams.size() & 1) != 0) {
		odd = 1;
	}

	unsigned int numrounds = mNumCycles * (odd ? teams.size() : teams.size() - 1);
	for(unsigned int j = 0; j < numrounds; j++) {
		Round r;

		for(unsigned int i = 0; i < (teams.size() + odd) / 2; i++) {
			unsigned int other = teams.size() + odd - 1 - i;
			if(other < teams.size()) {
				unsigned int ind1, ind2;
				if(j & 1) {
					ind1 = other;
					ind2 = i;
				}
				else {
					ind1 = i;
					ind2 = other;
				}

				r.addMatch(boost::shared_ptr<Match>(new Match(teams[ind1], teams[ind2], MatchRules(false, false, false))));
				printf("%5d-%-5d ", teams[ind1]->getId(), teams[ind2]->getId());
			}
		}
		printf("\n");
		mSchedule.addRound(r);
		std::rotate(teams.begin() + 1, teams.begin() + 2, teams.end());
	}
}

StatefulLeague::StatefulLeague()
	: mPointsPerWin(3),
	mNumCycles(2)
{
}

CompetitionType StatefulLeague::getType() const
{
	return CompetitionType::League;
}

unsigned int StatefulLeague::getNumberOfTeams() const
{
	return mEntries.size();
}

std::vector<boost::shared_ptr<StatefulTeam>> StatefulLeague::getTeamsByPosition() const
{
	std::vector<std::pair<boost::shared_ptr<StatefulTeam>, LeagueEntry>> ves(mEntries.begin(), mEntries.end());
	std::sort(ves.begin(), ves.end(), [](const std::pair<boost::shared_ptr<StatefulTeam>, LeagueEntry>& p1,
				const std::pair<boost::shared_ptr<StatefulTeam>, LeagueEntry>& p2) -> bool {
			if(p1.second.Points != p2.second.Points)
				return p1.second.Points > p2.second.Points;
			int gd1 = p1.second.GoalsFor - p1.second.GoalsAgainst;
			int gd2 = p2.second.GoalsFor - p2.second.GoalsAgainst;
			if(gd1 != gd2)
				return gd1 > gd2;
			if(p1.second.GoalsFor != p2.second.GoalsFor)
				return p1.second.GoalsFor > p2.second.GoalsFor;
			return strcmp(p1.first->getName().c_str(),
				p2.first->getName().c_str()) < 0;
			});

	std::vector<boost::shared_ptr<StatefulTeam>> res;
	for(auto t : ves)
		res.push_back(t.first);
	return res;
}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::StatefulLeague);


