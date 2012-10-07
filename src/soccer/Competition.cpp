#include <algorithm>
#include "League.h"

namespace Soccer {

StatefulCompetition::StatefulCompetition()
	: mNextMatch(boost::shared_ptr<Match>()),
	mThisRound(0),
	mNextMatchId(0)
{
}

void StatefulCompetition::setNextMatch()
{
	Round* r;

	r = mSchedule.getRound(mThisRound);
	if(!r) {
		mNextMatch = boost::shared_ptr<Match>();
		return;
	}

	mNextMatch = r->getMatch(mNextMatchId);
	if(!mNextMatch) {
		mThisRound++;
		mNextMatchId = 0;
		r = mSchedule.getRound(mThisRound);
		if(!r)
			return;
		mNextMatch = r->getMatch(mNextMatchId);
	}

	mNextMatchId++;
}

const Schedule& StatefulCompetition::getSchedule() const
{
	return mSchedule;
}

const boost::shared_ptr<Match> StatefulCompetition::getNextMatch() const
{
	return mNextMatch;
}

const Round* StatefulCompetition::getCurrentRound() const
{
	return mSchedule.getRound(mThisRound);
}

}
