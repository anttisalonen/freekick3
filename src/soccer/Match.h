#ifndef SOCCER_MATCH_H
#define SOCCER_MATCH_H

#include <map>
#include <memory>

#include "soccer/PlayerTactics.h"
#include "soccer/Team.h"
#include "soccer/Player.h"

namespace Soccer {

struct MatchResult {
	MatchResult() : HomeGoals(0), AwayGoals(0), Played(false) { }
	MatchResult(int h, int a) : HomeGoals(h), AwayGoals(a), Played(true) { }
	int HomeGoals;
	int AwayGoals;
	bool Played;
};

class SimulationStrength {
	public:
		SimulationStrength(const StatefulTeam& t);
		MatchResult simulateAgainst(const SimulationStrength& t2);

	private:
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

class Match {
	public:
		Match(const std::shared_ptr<StatefulTeam> t1, const std::shared_ptr<StatefulTeam> t2);
		MatchResult play(bool display) const;
		const MatchResult& getResult() const;
		void setResult(const MatchResult& m);
		const std::shared_ptr<StatefulTeam> getTeam(int i) const;

	private:
		static void playMatch(const char* datafile, int teamnum, int playernum);
		MatchResult simulateMatchResult() const;

		const std::shared_ptr<StatefulTeam> mTeam1;
		const std::shared_ptr<StatefulTeam> mTeam2;
		MatchResult mResult;
};

}

#endif

