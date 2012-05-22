#include "soccer/Match.h"

namespace Soccer {

Match::Match(const std::shared_ptr<StatefulTeam> t1, const std::shared_ptr<StatefulTeam> t2)
	: mTeam1(t1),
	mTeam2(t2)
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

const std::shared_ptr<StatefulTeam> Match::getTeam(int i) const
{
	if(i == 0)
		return mTeam1;
	else
		return mTeam2;
}

}


