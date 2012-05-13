#include "soccer/Match.h"

namespace Soccer {

TeamTactics::TeamTactics()
{
	for(int i = 1; i <= 11; i++)
		mTactics[i] = PlayerTactics(i > 5);
}

Match::Match(const std::shared_ptr<Team> t1, const std::shared_ptr<Team> t2,
		const TeamTactics& tt1,
		const TeamTactics& tt2)
	: mTeam1(t1),
	mTeam2(t2),
	mTeamTactics1(tt1),
	mTeamTactics2(tt2)
{
}

void Match::play()
{
}

const MatchResult& Match::getResult() const
{
	return mResult;
}

void Match::setResult(const MatchResult& m)
{
	mResult = m;
}

const std::shared_ptr<Team> Match::getTeam(int i) const
{
	if(i == 0)
		return mTeam1;
	else
		return mTeam2;
}

}


