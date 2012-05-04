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

void MatchEntity::setPosition(const AbsVector3& v)
{
	mPosition.v = v.v;
}

void MatchEntity::setVelocity(const AbsVector3& v)
{
	mVelocity.v = v.v;
}

void MatchEntity::setAcceleration(const AbsVector3& v)
{
	mAcceleration.v = v.v;
}

const AbsVector3& MatchEntity::getPosition() const
{
	return mPosition;
}

const AbsVector3& MatchEntity::getVelocity() const
{
	return mVelocity;
}

void MatchEntity::update(float time)
{
	mPosition.v += mVelocity.v * time;
	mVelocity.v += mAcceleration.v * time;
	mAcceleration = AbsVector3();
}


