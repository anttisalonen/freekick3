#include "match/MatchEntity.h"
#include "match/Match.h"

MatchEntity::MatchEntity(Match* match, bool ball, const Common::Vector3& pos)
	: Common::Vehicle(ball ? 0.2f : 0.9f, 100.0f, 100.0f, false), mMatch(match)
{
	mPosition = pos;
}

const Match* MatchEntity::getMatch() const
{
	return mMatch;
}


