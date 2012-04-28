#include "MatchEntity.h"
#include "Match.h"

MatchEntity::MatchEntity(Match* match, const AbsVector3& pos)
	: mMatch(match),
	mPosition(pos)
{
}

const Match* MatchEntity::getMatch() const
{
	return mMatch;
}

void MatchEntity::move(const AbsVector3& v)
{
	mPosition.v += v.v;
}

const AbsVector3& MatchEntity::getPosition() const
{
	return mPosition;
}


