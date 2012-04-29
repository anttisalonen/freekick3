#include "Ball.h"
#include "Match.h"

Ball::Ball(Match* match)
	: MatchEntity(match, Vector3(0, 0, 0))
{
}

void Ball::update(float time)
{
	MatchEntity::update(time);
	if(mPosition.v.z < 0.1f)
		mVelocity.v *= 1.0f - time * mMatch->getRollInertiaFactor();
}

