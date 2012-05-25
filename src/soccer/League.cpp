#include <algorithm>
#include "League.h"

namespace Soccer {

StatefulLeague::StatefulLeague(std::vector<std::shared_ptr<StatefulTeam>>& teams)
	: mNextMatch(nullptr),
	mThisRound(0),
	mNextMatchId(0),
	mGoalsPerWin(3)
{
	setRoundRobin(teams);
	setNextMatch();
}

void StatefulLeague::setNextMatch()
{
	Round* r;

	r = mSchedule.getRound(mThisRound);
	if(!r) {
		mNextMatch = nullptr;
		return;
	}

	mNextMatch = r->getMatch(mNextMatchId);
	if(!mNextMatch) {
		mThisRound++;
		mNextMatchId = 0;
		r = mSchedule.getRound(mThisRound);
		if(!r)
			return;
		mNextMatch = r->getMatch(mNextMatchId);
	}

	mNextMatchId++;
}

bool StatefulLeague::nextMatch(std::function<MatchResult (const Match& v)> func)
{
	if(!mNextMatch)
		return true;

	MatchResult res = func(*mNextMatch);
	if(!res.Played) {
		return false;
	}
	mNextMatch->setResult(res);
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
		meit1->second.Points += mGoalsPerWin;
		meit1->second.Wins++;
		meit2->second.Losses++;
	}
	else if(res.AwayGoals > res.HomeGoals) {
		meit2->second.Points += mGoalsPerWin;
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
	printf("Round: %d - Match: %d\n", mThisRound + 1, mNextMatchId + 1);
	return mNextMatch == nullptr;
}

const Schedule& StatefulLeague::getSchedule() const
{
	return mSchedule;
}

const std::map<std::shared_ptr<StatefulTeam>, LeagueEntry>& StatefulLeague::getEntries() const
{
	return mEntries;
}

void StatefulLeague::setRoundRobin(std::vector<std::shared_ptr<StatefulTeam>>& teams)
{
	mEntries.clear();
	for(auto t : teams) {
		mEntries.insert(std::make_pair(t, LeagueEntry()));
	}

	int odd = 0;
	if((teams.size() & 1) != 0) {
		odd = 1;
	}

	unsigned int numrounds = odd ? teams.size() : teams.size() - 1;
	for(unsigned int j = 0; j < numrounds; j++) {
		Round r;

		for(unsigned int i = 0; i < (teams.size() + odd) / 2; i++) {
			unsigned int other = teams.size() + odd - 1 - i;
			if(other < teams.size()) {
				r.addMatch(std::shared_ptr<Match>(new Match(teams[i], teams[other])));
				printf("%5d-%-5d ", teams[i]->getId(), teams[other]->getId());
			}
		}
		printf("\n");
		mSchedule.addRound(r);
		std::rotate(teams.begin() + 1, teams.begin() + 2, teams.end());
	}
}

std::shared_ptr<Match> StatefulLeague::getNextMatch() const
{
	return mNextMatch;
}

}
