#include "common/Math.h"
#include "common/Vector3.h"

#include "match/Ball.h"
#include "match/Match.h"
#include "match/Player.h"
#include "match/Team.h"

using Common::Vector3;

static const float collisionIgnoreDistance = 1.5f;

Ball::Ball(Match* match)
	: MatchEntity(match, Common::Vector3(0, 0, 0)),
	mGrabbed(false),
	mGrabber(nullptr)
{
	mPosition.z = 0.10f;
}

const Player* Ball::checkPlayerCollisions()
{
	const Player* ret = nullptr;
	if(mVelocity.length() > 2.0f &&
			(mPosition - mCollisionFreePoint).length() > collisionIgnoreDistance) {
		for(int i = 0; i < 2; i++) {
			for(auto p : mMatch->getTeam(i)->getPlayers()) {
				if(checkCollision(*p))
					ret = p.get();
			}
		}
	}
	return ret;
}

void Ball::update(float time)
{
	if(!mGrabbed) {
		bool outsideBefore1 = mPosition.x < -GOAL_WIDTH_2 - GOAL_NET_RADIUS;
		bool outsideBefore2 = mPosition.x > GOAL_WIDTH_2 + GOAL_NET_RADIUS;
		bool outsideBefore3 = mPosition.z > GOAL_HEIGHT;
		MatchEntity::update(time);
		bool outsideAfter1 = mPosition.x < -GOAL_WIDTH_2 - GOAL_NET_RADIUS;
		bool outsideAfter2 = mPosition.x > GOAL_WIDTH_2 + GOAL_NET_RADIUS;
		bool outsideAfter3 = mPosition.z > GOAL_HEIGHT;

		if(mPosition.z < 0.15f) {
			if(fabs(mVelocity.z) < 0.1f)
				mVelocity.z = 0.0f;

			if(mVelocity.z < -0.1f) {
				// bounciness
				/* TODO: this constant should be a pitch/match property. */
				mVelocity.z *= -0.65f;
			}
			else {
				mVelocity *= 1.0f - time * mMatch->getRollInertiaFactor();
			}
		}
		else {
			mVelocity *= 1.0f - time * mMatch->getAirViscosityFactor();
			mVelocity.z -= 9.81f * time;
		}

		{
			static_assert(GOAL_POST_RADIUS > GOAL_NET_RADIUS, "Post radius must be more than net radius.");
			bool xOnPost = fabs(mPosition.x - GOAL_WIDTH_2) < GOAL_POST_RADIUS;
			bool yOnPost = fabs(mPosition.y - mMatch->getPitchHeight() * 0.5f) < GOAL_POST_RADIUS;
			bool zOnPost = fabs(mPosition.z - GOAL_HEIGHT) < GOAL_POST_RADIUS;
			if((xOnPost && yOnPost && mPosition.z < GOAL_HEIGHT + GOAL_POST_RADIUS) ||
					(yOnPost && zOnPost && mPosition.x < GOAL_WIDTH_2 + GOAL_POST_RADIUS)) {
				// post/bar

				// keep ball on the pitch
				if(mPosition.y > 0.0f) {
					mVelocity.y = -fabs(mVelocity.y);
					mPosition.y = mMatch->getPitchHeight() * 0.5f - GOAL_POST_RADIUS - BALL_RADIUS * 1.2f;
				}
				else {
					mVelocity.y = fabs(mVelocity.y);
					mPosition.y = -mMatch->getPitchHeight() * 0.5f + GOAL_POST_RADIUS + BALL_RADIUS * 1.2f;
				}
				mVelocity *= 0.9f;
			}
			else {
				// net
				mPosition.y = Common::clamp(-mMatch->getPitchHeight() * 0.5f - 3.0f,
						mPosition.y,
						mMatch->getPitchHeight() * 0.5f + 3.0f);
				if(mPosition.y > mMatch->getPitchHeight() * 0.5f ||
						mPosition.y < -mMatch->getPitchHeight() * 0.5f) {
					/* check for ball hitting the net */
					if(mPosition.z < (GOAL_HEIGHT + GOAL_NET_RADIUS) &&
							(outsideBefore1 != outsideAfter1 || outsideBefore2 != outsideAfter2)) {
						mVelocity.x = 0.0f;

						// keep ball on the correct side of the net
						if(outsideBefore1 != outsideAfter1) {
							// left net
							mPosition.x = -GOAL_WIDTH_2;
							if(outsideBefore1) {
								// outside the goal
								mPosition.x -= 0.3f;
							}
							else {
								// inside the goal
								mPosition.x += 0.3f;
							}
						}
						else {
							// right net
							mPosition.x = GOAL_WIDTH_2;
							if(outsideBefore2) {
								// outside the goal
								mPosition.x += 0.3f;
							}
							else {
								// inside the goal
								mPosition.x -= 0.3f;
							}
						}
					}
					if(outsideBefore3 != outsideAfter3) {
						mVelocity.z = 0.0f;
						if(outsideBefore3) {
							/* keep ball on top of goal */
							mPosition.z = GOAL_HEIGHT + 0.01f;
						}
					}
				}
			}
		}
	}
	else {
		mAcceleration = Vector3();
		mVelocity = mGrabber->getVelocity();
		mPosition = mGrabber->getPosition();
	}
}

void Ball::kicked(Player* p)
{
	if(mGrabbed && mGrabber != p)
		return;
	mCollisionFreePoint = p->getPosition();
	mGrabbed = false;
	mGrabber = nullptr;
}

bool Ball::checkCollision(const Player& p)
{
	float dist = MatchEntity::distanceBetween(*this, p);
	if(dist < 1.0f) {
		bool catchSuccessful = getVelocity().length() / 80.0f < p.getSkills().BallControl;
		if(catchSuccessful)
			mVelocity *= -0.1f;
		else
			mVelocity *= -0.7f;
		return true;
	}
	else {
		return false;
	}
}

bool Ball::grabbed() const
{
	return mGrabbed;
}

void Ball::grab(Player* p)
{
	mGrabbed = true;
	mAcceleration = Vector3();
	mVelocity = Vector3();
	mGrabber = p;
	mPosition = mGrabber->getPosition();
	mPosition.z = 0.10f;
}

void Ball::drop()
{
	mGrabbed = false;
	mGrabber = nullptr;
}

const Player* Ball::getGrabber() const
{
	return mGrabber;
}


