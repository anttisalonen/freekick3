#ifndef SOCCER_MATCH_H
#define SOCCER_MATCH_H

#include <map>
#include <boost/shared_ptr.hpp>

#include "soccer/PlayerTactics.h"
#include "soccer/Team.h"
#include "soccer/Player.h"

namespace Soccer {

class MatchRules {
	public:
		MatchRules(bool et, bool pen, bool ag, unsigned int hga = 0, unsigned int aga = 0)
			: ExtraTimeOnTie(et),
			PenaltiesOnTie(pen),
			AwayGoals(ag),
			HomeAggregate(hga),
			AwayAggregate(aga)
	{ }
		const bool ExtraTimeOnTie = false;
		const bool PenaltiesOnTie = false;
		const bool AwayGoals = false;
		unsigned int HomeAggregate = 0;
		unsigned int AwayAggregate = 0;

	private:
		friend class boost::serialization::access;
		MatchRules();
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & const_cast<bool&>(ExtraTimeOnTie);
			ar & const_cast<bool&>(PenaltiesOnTie);
			ar & const_cast<bool&>(AwayGoals);
			ar & HomeAggregate;
			ar & AwayAggregate;
		}
};

struct MatchResult {
	MatchResult() : Played(false) { }
	MatchResult(unsigned int h, unsigned int a, unsigned int hp = 0, unsigned int ap = 0) :
		HomeGoals(h), AwayGoals(a),
		HomePenalties(hp), AwayPenalties(ap),
		Played(true) { }

	unsigned int HomeGoals = 0;
	unsigned int AwayGoals = 0;
	unsigned int HomePenalties = 0;
	unsigned int AwayPenalties = 0;
	bool Played;

	friend class boost::serialization::access;
	template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & HomeGoals;
			ar & AwayGoals;
			ar & HomePenalties;
			ar & AwayPenalties;
			ar & Played;
		}
};

class CupEntry {
	public:
		void addMatchResult(const MatchResult& m);
		unsigned int numMatchesPlayed() const;
		bool firstWon() const;
		std::pair<int, int> aggregate() const;
		std::pair<int, int> penalties() const;

	private:
		bool firstWinsByAwayGoals() const;

		std::vector<MatchResult> mMatchResults;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mMatchResults;
		}
};

class SimulationStrength {
	public:
		SimulationStrength(const StatefulTeam& t);
		MatchResult simulateAgainst(const SimulationStrength& t2, const MatchRules& r);

	private:
		void simulateStep(const SimulationStrength& t2, unsigned int& homegoals, unsigned int& awaygoals, const std::vector<float>& tries);

		static int pickOne(const std::vector<float>& values);
		float mCenterDefense;
		float mCenterGet;
		float mCenterUse;
		float mLeftDefense;
		float mLeftGet;
		float mLeftUse;
		float mRightDefense;
		float mRightGet;
		float mRightUse;

		float mCenterTry;
		float mLeftTry;
		float mRightTry;

		float mLongBalls;
};

class RunningMatch {
	public:
		RunningMatch(const Match& m);
		bool matchFinished(MatchResult* r);

	private:
		void startMatch(int teamnum, int playernum, const MatchRules& rules);
		pid_t mChildPid;
		char matchfilenamebuf[L_tmpnam];
};

class Match {
	public:
		Match(const boost::shared_ptr<StatefulTeam> t1, const boost::shared_ptr<StatefulTeam> t2,
				const MatchRules& r);
		MatchResult play(bool display) const;
		RunningMatch startMatch(bool display) const;
		const MatchResult& getResult() const;
		void setResult(const MatchResult& m);
		const boost::shared_ptr<StatefulTeam> getTeam(int i) const;
		const MatchRules& getRules() const;
		MatchRules& getRules();
		void setCupEntry(const CupEntry& c);
		const CupEntry& getCupEntry() const;
		static void setMatchDataDumpDirectory(const std::string& s);

	private:
		MatchResult simulateMatchResult() const;

		static std::string MatchDataDumpDirectory;

		const boost::shared_ptr<StatefulTeam> mTeam1;
		const boost::shared_ptr<StatefulTeam> mTeam2;
		MatchRules mRules;
		MatchResult mResult;
		CupEntry mCupEntry;

		friend class boost::serialization::access;
		Match();
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & const_cast<boost::shared_ptr<StatefulTeam>&>(mTeam1);
			ar & const_cast<boost::shared_ptr<StatefulTeam>&>(mTeam2);
			ar & mRules;
			ar & mResult;
			ar & mCupEntry;
		}
};

}

#endif

