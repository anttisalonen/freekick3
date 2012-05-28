#include "common/Math.h"

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
	mPosition.v.z = 0.10f;
}

void Ball::update(float time)
{
	if(!mGrabbed) {
		bool outsideBefore1 = mPosition.v.x < -GOAL_WIDTH_2 + GOAL_NET_RADIUS;
		bool outsideBefore2 = mPosition.v.x > GOAL_WIDTH_2 - GOAL_NET_RADIUS;
		bool outsideBefore3 = mPosition.v.z > GOAL_HEIGHT;
		MatchEntity::update(time);
		bool outsideAfter1 = mPosition.v.x < -GOAL_WIDTH_2 + GOAL_NET_RADIUS;
		bool outsideAfter2 = mPosition.v.x > GOAL_WIDTH_2 - GOAL_NET_RADIUS;
		bool outsideAfter3 = mPosition.v.z > GOAL_HEIGHT;

		if(mVelocity.v.length() > 2.0f &&
				(mPosition.v - mCollisionFreePoint.v).length() > collisionIgnoreDistance) {
			for(int i = 0; i < 2; i++) {
				for(auto p : mMatch->getTeam(i)->getPlayers()) {
					checkCollision(*p);
				}
			}
		}
		if(mPosition.v.z < 0.15f) {
			if(fabs(mVelocity.v.z) < 0.1f)
				mVelocity.v.z = 0.0f;

			if(mVelocity.v.z < -0.1f) {
				// bounciness
				/* TODO: this constant should be a pitch/match property. */
				mVelocity.v.z *= -0.65f;
			}
			else {
				mVelocity.v *= 1.0f - time * mMatch->getRollInertiaFactor();
			}
		}
		else {
			mVelocity.v *= 1.0f - time * mMatch->getAirViscosityFactor();
			mVelocity.v.z -= 9.81f * time;
		}

		{
			static_assert(GOAL_POST_RADIUS > GOAL_NET_RADIUS, "Post radius must be more than net radius.");
			bool xOnPost = fabs(mPosition.v.x - GOAL_WIDTH_2) < GOAL_POST_RADIUS;
			bool yOnPost = fabs(mPosition.v.y - mMatch->getPitchHeight() * 0.5f) < GOAL_POST_RADIUS;
			bool zOnPost = fabs(mPosition.v.z - GOAL_HEIGHT) < GOAL_POST_RADIUS;
			if((xOnPost && yOnPost && mPosition.v.z < GOAL_HEIGHT + GOAL_POST_RADIUS) ||
					(yOnPost && zOnPost && mPosition.v.x < GOAL_WIDTH_2 + GOAL_POST_RADIUS)) {
				// post/bar
				mVelocity.v.y = -mVelocity.v.y;
				mVelocity.v *= 0.9f;

				// keep ball on the pitch
				mPosition.v.y = mPosition.v.y > 0.0f ?
					mMatch->getPitchHeight() * 0.5f : -mMatch->getPitchHeight() * 0.5f;
			}
			else {
				// net
				mPosition.v.y = Common::clamp(-mMatch->getPitchHeight() * 0.5f - 3.0f,
						mPosition.v.y,
						mMatch->getPitchHeight() * 0.5f + 3.0f);
				if(mPosition.v.y > mMatch->getPitchHeight() * 0.5f ||
						mPosition.v.y < -mMatch->getPitchHeight() * 0.5f) {
					/* check for ball hitting the net */
					if(mPosition.v.z < (GOAL_HEIGHT + GOAL_NET_RADIUS) &&
							(outsideBefore1 != outsideAfter1 || outsideBefore2 != outsideAfter2)) {
						mVelocity.v.x = 0.0f;

						// keep ball on the correct side of the net
						if(outsideBefore1 != outsideAfter1) {
							mPosition.v.x = -GOAL_WIDTH_2 - 0.1f;
						}
						else {
							mPosition.v.x = GOAL_WIDTH_2 + 0.1f;
						}
					}
					if(outsideBefore3 != outsideAfter3) {
						mVelocity.v.z = 0.0f;
						if(outsideBefore3) {
							/* keep ball on top of goal */
							mPosition.v.z = GOAL_HEIGHT + 0.01f;
						}
					}
				}
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
	mPosition = mGrabber->getPosition();
	mPosition.v.z = 0.10f;
}

const Player* Ball::getGrabber() const
{
	return mGrabber;
}


