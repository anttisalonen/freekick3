#include <stdexcept>

#include "Cup.h"
#include "Season.h"

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


StatefulCup::StatefulCup(std::vector<boost::shared_ptr<StatefulTeam>>& teams, bool onlyoneround,
		unsigned int legs, bool awaygoals)
	: StatefulCompetition(),
	mOnlyOneRound(onlyoneround),
	mLegs(legs),
	mAwayGoals(awaygoals)
{
	mTotalRounds = logbase2(teams.size());
	setupNextRound(teams);
	setNextMatch();
}

void StatefulCup::matchPlayed(const MatchResult& res)
{
	assert(mNextMatch);
	assert(res.Played);

	auto it = mEntries.find({mNextMatch->getTeam(0), mNextMatch->getTeam(1)});
	if(it == mEntries.end())
		it = mEntries.find({mNextMatch->getTeam(1), mNextMatch->getTeam(0)});
	assert(it != mEntries.end());

	it->second.addMatchResult(res);
	mNextMatch->setCupEntry(it->second);

	if(res.HomeGoals == res.AwayGoals) {
		if(mLegs == 1) {
			assert(res.HomePenalties != res.AwayPenalties);
		}
	}

	setNextMatch();
	if(!mNextMatch && mEntries.size() > 1 && !mOnlyOneRound) {
		std::vector<boost::shared_ptr<StatefulTeam>> teams = getTeamsByPosition();
		assert(teams.size());
		setupNextRound(teams);
		setNextMatch();
	}

	if(mNextMatch) {
		auto newit = mEntries.find({mNextMatch->getTeam(0), mNextMatch->getTeam(1)});
		if(newit == mEntries.end())
			newit = mEntries.find({mNextMatch->getTeam(1), mNextMatch->getTeam(0)});
		assert(newit != mEntries.end());

		auto agg = newit->second.aggregate();
		mNextMatch->getRules().AwayAggregate = agg.first;
		mNextMatch->getRules().HomeAggregate = agg.second;
	}
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

	for(unsigned int i = 0; i < mLegs; i++) {
		Round r;
		for(auto it = teams.begin(); it != teams.end(); ++it) {
			auto t1 = *it;
			++it;
			assert(it != teams.end());
			auto t2 = *it;
			if(i == 0)
				mEntries.insert({{t1, t2}, CupEntry()});

			auto pen = (i == mLegs - 1);
			bool swapHomeAway = (i & 1) == 1;
			auto m = boost::shared_ptr<Match>(new Match(swapHomeAway ? t2 : t1, swapHomeAway ? t1 : t2,
						MatchRules(pen, pen, mAwayGoals)));
			r.addMatch(m);
		}
		mSchedule.addRound(r);
	}
}

StatefulCup::StatefulCup()
{
}

std::vector<boost::shared_ptr<StatefulTeam>> StatefulCup::collectTeamsFromCountry(const boost::shared_ptr<StatefulLeagueSystem> s)
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;
	unsigned int numTeams = 0;
	for(auto league : s->getLeagues()) {
		numTeams += league->getNumberOfTeams();
	}

	numTeams = pow2rounddown(numTeams);

	for(auto league : s->getLeagues()) {
		if(numTeams == 0)
			break;
		for(auto t : league->getTeamsByPosition()) {
			if(numTeams == 0)
				break;
			teams.push_back(t);
			numTeams--;
		}
	}

	return teams;
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

unsigned int StatefulCup::getNumberOfTeams() const
{
	return mEntries.size() * 2;
}

std::vector<boost::shared_ptr<StatefulTeam>> StatefulCup::getTeamsByPosition() const
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;

	for(auto& e : mEntries) {
		if(e.second.numMatchesPlayed() < mLegs) {
			return std::vector<boost::shared_ptr<StatefulTeam>>();
		}

		if(e.second.firstWon()) {
			teams.push_back(e.first.first);
		} else {
			teams.push_back(e.first.second);
		}
	}

	return teams;
}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Soccer::StatefulCup);

