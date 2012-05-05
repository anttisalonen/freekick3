#include "Ball.h"
#include "Match.h"
#include "Player.h"

static const float collisionIgnoreDistance = 1.5f;

Ball::Ball(Match* match)
	: MatchEntity(match, Vector3(0, 0, 0)),
	mGrabbed(false),
	mGrabber(nullptr)
{
}

void Ball::update(float time)
{
	if(!mGrabbed) {
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
	else {
		mAcceleration = AbsVector3();
		mVelocity = mGrabber->getVelocity();
		mPosition = mGrabber->getPosition();
	}
}

void Ball::kicked(Player* p)
{
	if(mGrabbed && mGrabber != p)
		return;
	mCollisionFreePoint = mPosition;
	mGrabbed = false;
	mGrabber = nullptr;
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

bool Ball::grabbed() const
{
	return mGrabbed;
}

void Ball::grab(Player* p)
{
	mGrabbed = true;
	mAcceleration = AbsVector3();
	mVelocity = AbsVector3();
	mGrabber = p;
}

const Player* Ball::getGrabber() const
{
	return mGrabber;
}


