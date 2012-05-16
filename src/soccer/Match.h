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

class TeamTactics {
	public:
		TeamTactics();
	private:
		/* TODO: decide and specify whether the key is player index or player ID */
		std::map<int, PlayerTactics> mTactics;
};

class Match {
	public:
		Match(const std::shared_ptr<Team> t1, const std::shared_ptr<Team> t2,
				const TeamTactics& tt1,
				const TeamTactics& tt2);
		void play();
		const MatchResult& getResult() const;
		void setResult(const MatchResult& m);
		const std::shared_ptr<Team> getTeam(int i) const;

	private:
		const std::shared_ptr<Team> mTeam1;
		const std::shared_ptr<Team> mTeam2;
		const TeamTactics& mTeamTactics1;
		const TeamTactics& mTeamTactics2;
		MatchResult mResult;
};

}

#endif

