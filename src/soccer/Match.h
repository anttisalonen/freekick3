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

class Match {
	public:
		Match(const std::shared_ptr<StatefulTeam> t1, const std::shared_ptr<StatefulTeam> t2);
		void play();
		const MatchResult& getResult() const;
		void setResult(const MatchResult& m);
		const std::shared_ptr<StatefulTeam> getTeam(int i) const;

	private:
		const std::shared_ptr<StatefulTeam> mTeam1;
		const std::shared_ptr<StatefulTeam> mTeam2;
		MatchResult mResult;
};

}

#endif

