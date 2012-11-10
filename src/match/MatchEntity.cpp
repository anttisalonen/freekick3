#include "match/MatchEntity.h"
#include "match/Match.h"

MatchEntity::MatchEntity(Match* match, const Common::Vector3& pos)
	: Common::Entity(), mMatch(match)
{
	mPosition = pos;
}

const Match* MatchEntity::getMatch() const
{
	return mMatch;
}


