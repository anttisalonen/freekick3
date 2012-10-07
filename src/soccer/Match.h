#ifndef SOCCER_MATCH_H
#define SOCCER_MATCH_H

#include <map>
#include <boost/shared_ptr.hpp>

#include "soccer/PlayerTactics.h"
#include "soccer/Team.h"
#include "soccer/Player.h"

namespace Soccer {

struct MatchRules {
	MatchRules(bool et, bool pen) : ExtraTimeOnTie(et),
		PenaltiesOnTie(pen) { }
	const bool ExtraTimeOnTie;
	const bool PenaltiesOnTie;

	private:
		friend class boost::serialization::access;
		MatchRules();
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & const_cast<bool&>(ExtraTimeOnTie);
			ar & const_cast<bool&>(PenaltiesOnTie);
		}
};

struct MatchResult {
	MatchResult() : HomeGoals(0), AwayGoals(0),
		HomePenalties(0), AwayPenalties(0), Played(false) { }
	MatchResult(int h, int a, int hp = 0, int ap = 0) : HomeGoals(h), AwayGoals(a),
		HomePenalties(hp), AwayPenalties(ap), Played(true) { }
	bool homeWon() const;
	bool awayWon() const;

	int HomeGoals;
	int AwayGoals;
	int HomePenalties;
	int AwayPenalties;
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

class SimulationStrength {
	public:
		SimulationStrength(const StatefulTeam& t);
		MatchResult simulateAgainst(const SimulationStrength& t2, const MatchRules& r);

	private:
		void simulateStep(const SimulationStrength& t2, int& homegoals, int& awaygoals, const std::vector<float>& tries);

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
		void startMatch(int teamnum, int playernum);
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

	private:
		MatchResult simulateMatchResult() const;

		const boost::shared_ptr<StatefulTeam> mTeam1;
		const boost::shared_ptr<StatefulTeam> mTeam2;
		MatchRules mRules;
		MatchResult mResult;

		friend class boost::serialization::access;
		Match();
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & const_cast<boost::shared_ptr<StatefulTeam>&>(mTeam1);
			ar & const_cast<boost::shared_ptr<StatefulTeam>&>(mTeam2);
			ar & mRules;
			ar & mResult;
		}
};

}

#endif

