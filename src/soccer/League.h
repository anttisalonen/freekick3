#ifndef SOCCERLEAGUE_H
#define SOCCERLEAGUE_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

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

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & Points;
		ar & GoalsFor;
		ar & GoalsAgainst;
		ar & Matches;
		ar & Wins;
		ar & Draws;
		ar & Losses;
	}
};

class Round {
	public:
		Round() { }
		void addMatch(boost::shared_ptr<Match> m)
		{
			mMatches.push_back(m);
		}
		const std::vector<boost::shared_ptr<Match>>& getMatches() const
		{
			return mMatches;
		}
		const boost::shared_ptr<Match> getMatch(unsigned int rn) const
		{
			if(rn >= mMatches.size())
				return boost::shared_ptr<Match>();
			else
				return mMatches[rn];
		}

		boost::shared_ptr<Match> getMatch(unsigned int rn)
		{
			if(rn >= mMatches.size())
				return boost::shared_ptr<Match>();
			else
				return mMatches[rn];
		}


	private:
		std::vector<boost::shared_ptr<Match>> mMatches;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mMatches;
		}
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

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mRounds;
		}
};

class StatefulLeague {
	public:
		StatefulLeague(std::vector<boost::shared_ptr<StatefulTeam>>& teams);
		bool nextMatch(std::function<MatchResult (const Match& v)> func);
		const Schedule& getSchedule() const;
		const std::map<boost::shared_ptr<StatefulTeam>, LeagueEntry>& getEntries() const;
		const boost::shared_ptr<Match> getNextMatch() const;
		const Round* getCurrentRound() const;

	private:
		void setRoundRobin(std::vector<boost::shared_ptr<StatefulTeam>>& teams);
		void setNextMatch();
		std::map<boost::shared_ptr<StatefulTeam>, LeagueEntry> mEntries;
		Schedule mSchedule;
		boost::shared_ptr<Match> mNextMatch;
		int mThisRound;
		int mNextMatchId;
		const int mPointsPerWin;
		const int mNumCycles;

		friend class boost::serialization::access;
		StatefulLeague(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mEntries;
			ar & mSchedule;
			ar & mNextMatch;
			ar & mThisRound;
			ar & mNextMatchId;
			ar & const_cast<int&>(mPointsPerWin);
			ar & const_cast<int&>(mNumCycles);
		}
};

}

#endif

