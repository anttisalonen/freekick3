#include "match/Math.h"
#include "match/Ball.h"
#include "match/Match.h"
#include "match/Player.h"
#include "match/Team.h"

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
		bool outsideBefore1 = mPosition.v.x < -3.96f;
		bool outsideBefore2 = mPosition.v.x > 3.96f;
		MatchEntity::update(time);
		bool outsideAfter1 = mPosition.v.x < -3.36f;
		bool outsideAfter2 = mPosition.v.x > 3.36f;

		if(mVelocity.v.length() > 2.0f &&
				(mPosition.v - mCollisionFreePoint.v).length() > collisionIgnoreDistance) {
			for(auto p : mMatch->getTeam(0)->getPlayers()) {
				checkCollision(*p);
			}
		}
		if(mPosition.v.z < 0.1f) {
			mVelocity.v *= 1.0f - time * mMatch->getRollInertiaFactor();
		}

		// net
		mPosition.v.y = clamp(-mMatch->getPitchHeight() * 0.5f - 3.0f,
				mPosition.v.y,
				mMatch->getPitchHeight() * 0.5f + 3.0f);
		if(mPosition.v.y > mMatch->getPitchHeight() * 0.5f || mPosition.v.y < -mMatch->getPitchHeight() * 0.5f) {
			if(outsideBefore1 != outsideAfter1 || outsideBefore2 != outsideAfter2) {
				mVelocity.v.zero();
			}
		}
		else if(mPosition.v.y > mMatch->getPitchHeight() * 0.5f - 0.5f ||
				mPosition.v.y < -mMatch->getPitchHeight() * 0.5f + 0.5f) {
			if(fabs(mPosition.v.x) < 3.69f && fabs(mPosition.v.x) > 3.63f) {
				// post
				mVelocity.v.y = -mVelocity.v.y;
				mVelocity.v *= 0.8f;
			}
		}
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


