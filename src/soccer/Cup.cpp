#include <stdexcept>
#include "Cup.h"

namespace Soccer {

/* thanks go to http://graphics.stanford.edu/~seander/bithacks.html */
static unsigned int logbase2(unsigned int v)
{
	unsigned int r = 0; // r will be lg(v)

	while (v >>= 1)
	{
		  r++;
	}

	return r;
}

static bool ispow2(unsigned int v)
{
	return v && !(v & (v - 1));
}

// compute the next highest power of 2 of 32-bit x
static unsigned int pow2roundup(unsigned int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

static unsigned int pow2rounddown(unsigned int x)
{
	if(ispow2(x))
		return x;

	return pow2roundup(x) / 2;
}

StatefulCup::StatefulCup(std::vector<boost::shared_ptr<StatefulTeam>>& teams)
	: StatefulCompetition()
{
	mTotalRounds = logbase2(teams.size());
	setupNextRound(teams);
	setNextMatch();
}

bool StatefulCup::matchPlayed(const MatchResult& res)
{
	if(!mNextMatch)
		return true;

	if(!res.Played) {
		return false;
	}
	mNextMatch->setResult(res);

	auto it = mEntries.find({mNextMatch->getTeam(0), mNextMatch->getTeam(1)});
	assert(it != mEntries.end());
	if(res.HomeGoals == res.AwayGoals) {
		assert(res.HomePenalties != res.AwayPenalties);
	}
	it->second.Result = res;

	setNextMatch();
	if(!mNextMatch && mEntries.size() > 1) {
		std::vector<boost::shared_ptr<StatefulTeam>> teams;
		for(auto& e : mEntries) {
			assert(e.second.Result.Played);
			if(e.second.Result.HomeGoals > e.second.Result.AwayGoals ||
					e.second.Result.HomePenalties > e.second.Result.AwayPenalties)
				teams.push_back(e.first.first);
			else
				teams.push_back(e.first.second);
		}
		setupNextRound(teams);
		setNextMatch();
	}
	return mNextMatch == boost::shared_ptr<Match>();
}

void StatefulCup::setupNextRound(std::vector<boost::shared_ptr<StatefulTeam>>& teams)
{
	if(!ispow2(teams.size())) {
		throw std::runtime_error("Number of teams in cup must be power of two.");
	}

	if(teams.size() == 1) {
		throw std::runtime_error("Number of teams in cup must not be one.");
	}

	mEntries.clear();
	std::random_shuffle(teams.begin(), teams.end());

	Round r;
	for(auto it = teams.begin(); it != teams.end(); ++it) {
		auto t1 = *it;
		++it;
		assert(it != teams.end());
		auto t2 = *it;
		mEntries.insert({{t1, t2}, CupEntry()});
		r.addMatch(boost::shared_ptr<Match>(new Match(t1, t2, MatchRules(true, true))));
	}
	mSchedule.addRound(r);
}

StatefulCup::StatefulCup()
{
}

std::vector<boost::shared_ptr<Team>> StatefulCup::collectTeamsFromCountry(const boost::shared_ptr<LeagueSystem> s)
{
	std::vector<boost::shared_ptr<Team>> teams;
	unsigned int numTeams = 0;
	for(auto league : s->getContainer()) {
		numTeams += league.second->getContainer().size();
	}

	numTeams = pow2rounddown(numTeams);

	for(auto league : s->getContainer()) {
		if(numTeams == 0)
			break;
		for(auto t : league.second->getContainer()) {
			if(numTeams == 0)
				break;
			teams.push_back(t.second);
			numTeams--;
		}
	}

	return teams;
}

CompetitionType StatefulCup::getType() const
{
	return CompetitionType::Cup;
}

unsigned int StatefulCup::getTotalNumberOfRounds() const
{
	return mTotalRounds;
}


}
