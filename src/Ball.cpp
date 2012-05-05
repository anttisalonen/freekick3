#include "Ball.h"
#include "Match.h"
#include "Player.h"

static const float collisionIgnoreDistance = 1.5f;

Ball::Ball(Match* match)
	: MatchEntity(match, Vector3(0, 0, 0))
{
}

void Ball::update(float time)
{
	MatchEntity::update(time);
	if(mVelocity.v.length() > 2.0f &&
			(mPosition.v - mCollisionFreePoint.v).length() > collisionIgnoreDistance) {
		for(auto p : mMatch->getTeam(0)->getPlayers()) {
			checkCollision(*p);
		}
	}
	if(mPosition.v.z < 0.1f)
		mVelocity.v *= 1.0f - time * mMatch->getRollInertiaFactor();
}

void Ball::kicked()
{
	mCollisionFreePoint = mPosition;
}

void Ball::checkCollision(const Player& p)
{
	float dist = MatchEntity::distanceBetween(*this, p);
	if(dist < 1.0f) {
		bool catchSuccessful = getVelocity().v.length() / 80.0f < p.getSkills().BallControl;
		if(catchSuccessful)
			mVelocity.v *= -0.1f;
		else
			mVelocity.v *= -0.7f;
	}
}


