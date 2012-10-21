#include <algorithm>

#include <boost/serialization/export.hpp>

#include "soccer/Tournament.h"

using namespace Common;

namespace Soccer {

GroupStage::GroupStage(unsigned int numGroups, unsigned int numTeams, unsigned int numWinners, unsigned int legs)
	: mNumGroups(numGroups),
	mNumTeams(numTeams),
	mNumWinners(numWinners),
	mLegs(legs)
{
}

unsigned int GroupStage::getNumberOfGroups() const
{
	return mNumGroups;
}

void GroupStage::addStage(StatefulTournament& t)
{
	t.addGroupStage(*this);
}

unsigned int GroupStage::getTotalTeams() const
{
	return mNumTeams;
}

unsigned int GroupStage::getContinuingTeams() const
{
	return mNumWinners;
}

unsigned int GroupStage::getNumberOfLegs() const
{
	return mLegs;
}

KnockoutStage::KnockoutStage(unsigned int legs, bool awaygoals, unsigned int continuingteams)
	: mContinuingTeams(continuingteams),
	mLegs(legs),
	mAwayGoals(awaygoals)
{
}

void KnockoutStage::addStage(StatefulTournament& t)
{
	t.addKnockoutStage(*this);
}

unsigned int KnockoutStage::getTotalTeams() const
{
	return mContinuingTeams * 2;
}

unsigned int KnockoutStage::getContinuingTeams() const
{
	return mContinuingTeams;
}

unsigned int KnockoutStage::getNumberOfLegs() const
{
	return mLegs;
}

bool KnockoutStage::getAwayGoals() const
{
	return mAwayGoals;
}

TournamentConfig::TournamentConfig()
{
}

void TournamentConfig::pushStage(boost::shared_ptr<TournamentStage> r)
{
	mStages.push_back(r);
}

StatefulTournamentStage::StatefulTournamentStage(const std::vector<boost::shared_ptr<StatefulCompetition>>& p)
	: mTournamentGroups(p),
	mCurrentGroupIndex(0)
{
	assert(!mTournamentGroups.empty());
}

const boost::shared_ptr<StatefulCompetition> StatefulTournamentStage::getCurrentTournamentGroup() const
{
	if(mTournamentGroups.empty()) {
		return boost::shared_ptr<StatefulCompetition>();
	} else {
		return mTournamentGroups[mCurrentGroupIndex];
	}
}

boost::shared_ptr<StatefulCompetition> StatefulTournamentStage::getCurrentTournamentGroup()
{
	if(mTournamentGroups.empty()) {
		return boost::shared_ptr<StatefulCompetition>();
	} else {
		return mTournamentGroups[mCurrentGroupIndex];
	}
}

const boost::shared_ptr<Match> StatefulTournamentStage::getNextMatch() const
{
	auto tr = getCurrentTournamentGroup();
	assert(tr);
	return tr->getNextMatch();
}

void StatefulTournamentStage::matchPlayed(const MatchResult& res)
{
	auto tr = getCurrentTournamentGroup();
	assert(tr);

	auto roundbefore = tr->getNextMatchRoundNumber();

	tr->matchPlayed(res);

	auto roundafter = tr->getNextMatchRoundNumber();
	if(roundbefore != roundafter || !tr->getNextMatch()) {
		mCurrentGroupIndex++;
		if(mCurrentGroupIndex >= mTournamentGroups.size())
			mCurrentGroupIndex = 0;
	}
}

CompetitionType StatefulTournamentStage::getType() const
{
	auto tr = getCurrentTournamentGroup();
	assert(tr);
	return tr->getType();
}

unsigned int StatefulTournamentStage::getNumberOfTeams() const
{
	unsigned int i = 0;
	for(auto t : mTournamentGroups) {
		i += t->getNumberOfTeams();
	}
	return i;
}

std::vector<boost::shared_ptr<StatefulTeam>> StatefulTournamentStage::getTeamsByPosition() const
{
	std::vector<std::vector<boost::shared_ptr<StatefulTeam>>> individualLists;
	for(auto t : mTournamentGroups) {
		individualLists.push_back(t->getTeamsByPosition());
	}

	std::vector<boost::shared_ptr<StatefulTeam>> ret;
	int maxTeams = 0;
	for(auto& l : individualLists) {
		if(l.size() > maxTeams)
			maxTeams = l.size();
	}

	for(unsigned int i = 0; i < maxTeams; i++) {
		for(auto& l : individualLists) {
			if(l.size() >= i) {
				ret.push_back(l[i]);
			}
		}
	}

	return ret;
}

const std::vector<boost::shared_ptr<StatefulCompetition>>& StatefulTournamentStage::getGroups() const
{
	return mTournamentGroups;
}

std::vector<boost::shared_ptr<Match>> StatefulTournamentStage::getCurrentRoundMatches() const
{
	std::vector<boost::shared_ptr<Match>> ret;
	for(auto t : mTournamentGroups) {
		auto m = t->getCurrentRoundMatches();
		ret.insert(ret.end(), m.begin(), m.end());
	}
	return ret;
}


StatefulTournament::StatefulTournament(const TournamentConfig& tc, std::vector<boost::shared_ptr<StatefulTeam>>& teams)
	: mTeams(teams),
	mConfig(tc)
{
	assert(tc.mStages.size());
	std::cout << tc.mStages.size() << " rounds in tournament.\n";
	mConfig.mStages.back()->addStage(*this);
	mConfig.mStages.pop_back();
}

void StatefulTournament::addGroupStage(const GroupStage& r)
{
	assert(r.getNumberOfGroups());
	std::vector<std::vector<boost::shared_ptr<StatefulTeam>>> groups;

	unsigned int g = 0;
	for(auto t : mTeams) {
		if(groups.size() <= g)
			groups.push_back(std::vector<boost::shared_ptr<StatefulTeam>>());
		groups[g].push_back(t);
		g++;
		if(g >= r.getNumberOfGroups())
			g = 0;
	}
	mTeams.clear();
	std::vector<boost::shared_ptr<StatefulCompetition>> competitions;

	for(auto& gr : groups) {
		boost::shared_ptr<StatefulLeague> league(new StatefulLeague(gr, r.getNumberOfLegs()));
		competitions.push_back(league);
	}
	mTournamentStages.push_back(boost::shared_ptr<StatefulTournamentStage>(new StatefulTournamentStage(competitions)));
}

void StatefulTournament::addKnockoutStage(const KnockoutStage& r)
{
	/* TODO: handle legs and away goals */
	assert(mTeams.size() == r.getTotalTeams());
	boost::shared_ptr<StatefulCup> cup(new StatefulCup(mTeams, true, r.getNumberOfLegs(), r.getAwayGoals()));
	mTournamentStages.push_back(boost::shared_ptr<StatefulTournamentStage>(new StatefulTournamentStage({boost::shared_ptr<StatefulCompetition>(cup)})));
	mTeams.clear();
}

const boost::shared_ptr<Match> StatefulTournament::getNextMatch() const
{
	auto tr = getCurrentStage();
	assert(tr);
	return tr->getNextMatch();
}

void StatefulTournament::matchPlayed(const MatchResult& res)
{
	auto tr = getCurrentStage();
	assert(tr);
	tr->matchPlayed(res);

	if(!tr->getNextMatch()) {
		if(!mConfig.mStages.empty()) {
			auto st = mConfig.mStages.back();
			mTeams = tr->getTeamsByPosition();
			assert(st->getTotalTeams() <= mTeams.size());
			if(st->getTotalTeams() != mTeams.size())
				mTeams.erase(mTeams.begin() + st->getTotalTeams(), mTeams.end());
			assert(!mTeams.empty());
			st->addStage(*this);
			mConfig.mStages.pop_back();
		}
	}
}

std::vector<boost::shared_ptr<StatefulTeam>> StatefulTournament::getTeamsByPosition() const
{
	auto tr = getCurrentStage();
	assert(tr);
	return tr->getTeamsByPosition();
}

CompetitionType StatefulTournament::getType() const
{
	return CompetitionType::Tournament;
}

const boost::shared_ptr<StatefulTournamentStage> StatefulTournament::getCurrentStage() const
{
	if(mTournamentStages.empty()) {
		return boost::shared_ptr<StatefulTournamentStage>();
	} else {
		return mTournamentStages.back();
	}
}

boost::shared_ptr<StatefulTournamentStage> StatefulTournament::getCurrentStage()
{
	if(mTournamentStages.empty()) {
		return boost::shared_ptr<StatefulTournamentStage>();
	} else {
		return mTournamentStages.back();
	}
}

std::vector<boost::shared_ptr<Match>> StatefulTournament::getCurrentRoundMatches() const
{
	auto tr = getCurrentStage();
	assert(tr);
	return tr->getCurrentRoundMatches();
}

StatefulTournament::StatefulTournament()
{
}


}


