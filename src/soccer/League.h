#ifndef SOCCERLEAGUE_H
#define SOCCERLEAGUE_H

#include <memory>
#include <vector>

#include "soccer/Match.h"


namespace Soccer {

struct LeagueEntry {
	LeagueEntry()
		: Points(0),
		GoalsFor(0),
		GoalsAgainst(0),
		Matches(0),
		Wins(0),
		Draws(0),
		Losses(0) { }
	int Points;
	int GoalsFor;
	int GoalsAgainst;
	int Matches;
	int Wins;
	int Draws;
	int Losses;
};

class Round {
	public:
		Round() { }
		void addMatch(std::shared_ptr<Match> m)
		{
			mMatches.push_back(m);
		}
		const std::vector<std::shared_ptr<Match>> getMatches() const
		{
			return mMatches;
		}
		const std::shared_ptr<Match> getMatch(unsigned int rn) const
		{
			if(rn >= mMatches.size())
				return nullptr;
			else
				return mMatches[rn];
		}

		std::shared_ptr<Match> getMatch(unsigned int rn)
		{
			if(rn >= mMatches.size())
				return nullptr;
			else
				return mMatches[rn];
		}


	private:
		std::vector<std::shared_ptr<Match>> mMatches;
};

class Schedule {
	public:
		Schedule() { }
		void addRound(const Round& r)
		{
			mRounds.push_back(r);
		}

		const Round* getRound(unsigned int rn) const
		{
			if(rn >= mRounds.size())
				return nullptr;
			else
				return &mRounds[rn];
		}

		Round* getRound(unsigned int rn)
		{
			if(rn >= mRounds.size())
				return nullptr;
			else
				return &mRounds[rn];
		}

	private:
		std::vector<Round> mRounds;
};

class StatefulLeague {
	public:
		StatefulLeague(std::vector<std::shared_ptr<StatefulTeam>>& teams);
		bool nextMatch(std::function<MatchResult (const Match& v)> func);
		const Schedule& getSchedule() const;
		const std::map<std::shared_ptr<StatefulTeam>, LeagueEntry>& getEntries() const;
		std::shared_ptr<Match> getNextMatch() const;

	private:
		void setRoundRobin(std::vector<std::shared_ptr<StatefulTeam>>& teams);
		void setNextMatch();
		std::map<std::shared_ptr<StatefulTeam>, LeagueEntry> mEntries;
		Schedule mSchedule;
		std::shared_ptr<Match> mNextMatch;
		int mThisRound;
		int mNextMatchId;
		const int mGoalsPerWin;
};

}

#endif

