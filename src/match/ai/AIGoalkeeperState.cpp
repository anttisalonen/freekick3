#include "common/Math.h"

#include "match/ai/AIHelpers.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIPlayStates.h"
#include "match/PlayerActions.h"

using Common::Vector3;

AIGoalkeeperState::AIGoalkeeperState(Player* p, AIPlayController* m)
	: AIState(p, m),
	mHoldBallTimer(1.0f)
{
	mDescription = "Goalkeeper";
	setPivotPoint();
}

boost::shared_ptr<PlayerAction> AIGoalkeeperState::actOnBall(double time)
{
	if(mPlayer->getMatch()->getBall()->grabbed() && mPlayer->getMatch()->getBall()->getGrabber() == mPlayer) {
		// holding the ball
		mHoldBallTimer.doCountdown(time);
		if(mHoldBallTimer.check()) {
			return mPlayController->switchState(boost::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
		}
		else {
			return boost::shared_ptr<PlayerAction>(new IdlePA());
		}
	}
	else {
		// not holding the ball
		if(!MatchHelpers::canGrabBall(*mPlayer)) {
			return mPlayController->switchState(boost::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
		}
		else {
			mHoldBallTimer.rewind();
			return boost::shared_ptr<PlayerAction>(new GrabBallPA());
		}
	}
}

boost::shared_ptr<PlayerAction> AIGoalkeeperState::actNearBall(double time)
{
	float balltogoaldist = (MatchHelpers::ownGoalPosition(*mPlayer) - mPlayer->getMatch()->getBall()->getPosition()).length();
	float ballvel = mPlayer->getMatch()->getBall()->getVelocity().length();
	float balltooppdist = (MatchHelpers::nearestOppositePlayerToBall(*mPlayer->getTeam())->getPosition() -
			MatchHelpers::ownGoalPosition(*mPlayer)).length();

	float disttonearestdef = MatchEntity::distanceBetween(*MatchHelpers::nearestOwnFieldPlayerToBall(*mPlayer->getTeam()),
			*mPlayer->getMatch()->getBall());
	float disttoball = MatchEntity::distanceBetween(*mPlayer, *mPlayer->getMatch()->getBall());
	float disttogoal = MatchHelpers::distanceToOwnGoal(*mPlayer);

	if(disttonearestdef < disttoball && disttogoal < 15.0f) {
		return AIHelpers::createMoveActionToBall(*mPlayer);
	}

	if(MatchHelpers::canGrabBall(*mPlayer) && ballvel > 1.0f) {
		mHoldBallTimer.rewind();
		return boost::shared_ptr<PlayerAction>(new GrabBallPA());
	}
	if(MatchHelpers::canKickBall(*mPlayer)) {
		return actOnBall(time);
	}
	else if((balltogoaldist < 5.0f &&
			ballvel < 5.0f) || (balltogoaldist < 8.0f && ballvel < 3.0f && balltooppdist > 5.0f)) {
		return AIHelpers::createMoveActionToBall(*mPlayer);
	}
	else {
		return actOffBall(time);
	}
}

boost::shared_ptr<PlayerAction> AIGoalkeeperState::actOffBall(double time)
{
	const Ball* ball = mPlayer->getMatch()->getBall();
	Vector3 ballpos = ball->getPosition();
	Vector3 futureballpos = ballpos + ball->getVelocity() * 0.5f;
	Vector3 goalmiddlepoint = MatchHelpers::ownGoalPosition(*mPlayer);
	static const float gkdisttogoal = 1.0f;
	if(MatchHelpers::attacksUp(*mPlayer))
		goalmiddlepoint.y += gkdisttogoal;
	else
		goalmiddlepoint.y -= gkdisttogoal;

	float balltowardsgoal = Common::Math::pointToLineDistance(ballpos, futureballpos, goalmiddlepoint);
	if(Common::signum(ball->getVelocity().y) == Common::signum(goalmiddlepoint.y) && balltowardsgoal < GOAL_WIDTH_2) {
		Vector3 tgtpos = Common::Math::lineLineIntersection2D(ballpos,
				futureballpos,
				Vector3(-GOAL_WIDTH_2 * 0.7f, goalmiddlepoint.y, 0),
				Vector3(GOAL_WIDTH_2 * 0.7f, goalmiddlepoint.y, 0));
		// tgtpos may be null when the ball doesn't move
		if(!tgtpos.null()) {
			float timetogoal = (ballpos - goalmiddlepoint).length() / ball->getVelocity().length();
			if(timetogoal < 0.5f && futureballpos.z > 2.0f &&
					(tgtpos - mPlayer->getPosition()).length() < 1.0f) {
				// jump
				tgtpos -= mPlayer->getPosition();
				tgtpos.z = fabs(futureballpos.z);
				return boost::shared_ptr<PlayerAction>(new JumpToPA(tgtpos));
			}
			else {
				return jumpToBall(time);
			}
		}
	}

	Vector3 vectoball = MatchEntity::vectorFromTo(*mPlayer, *mPlayer->getMatch()->getBall());
	if(vectoball.length() < 10.0f) {
		Vector3 vectogoal = MatchHelpers::ownGoalPosition(*mPlayer) - mPlayer->getPosition();
		if(vectogoal.length() < 10.0f) {
			return AIHelpers::createMoveActionToBall(*mPlayer);
		}
	}
	Vector3 diffvec = ballpos - mPivotPoint;
	Vector3 point = mPivotPoint + diffvec.normalized() * mDistanceFromPivot;
	point.x = Common::clamp(-3.0f, point.x, 3.0f);
	if(mPivotPoint.y > 0)
		point.y = std::min(mPlayer->getMatch()->getPitchHeight() * 0.5f, point.y);
	else
		point.y = std::max(mPlayer->getMatch()->getPitchHeight() * -0.5f, point.y);

	return AIHelpers::createMoveActionTo(*mPlayer, point);
}

void AIGoalkeeperState::matchHalfChanged(MatchHalf m)
{
	setPivotPoint();
}

void AIGoalkeeperState::setPivotPoint()
{
	if(MatchHelpers::attacksUp(*mPlayer)) {
		mPivotPoint = mPlayer->getMatch()->convertRelativeToAbsoluteVector(RelVector3(0, -1.2, 0));
	}
	else {
		mPivotPoint = mPlayer->getMatch()->convertRelativeToAbsoluteVector(RelVector3(0, 1.2, 0));
	}
	mDistanceFromPivot = mPlayer->getMatch()->getPitchHeight() * 0.13f;
}

boost::shared_ptr<PlayerAction> AIGoalkeeperState::jumpToBall(double time)
{
	if(!MatchHelpers::grabBallAllowed(*mPlayer)) {
		return AIHelpers::createMoveActionToBall(*mPlayer);
	} else {
		// run, jump or dive
		const Ball* b = mPlayer->getMatch()->getBall();
		auto ballpos = b->getPosition();
		auto futureBallPos = ballpos + b->getVelocity() * 0.5f;
		auto myPos = mPlayer->getPosition();
		Vector3 intersectionPoint;
		Common::Math::pointToSegmentDistance(ballpos, futureBallPos, myPos, &intersectionPoint);

		Vector3 jumpVector;
		if(intersectionPoint.null()) {
			jumpVector = ballpos;
		} else {
			jumpVector = intersectionPoint;
		}
		jumpVector -= myPos;
		if(jumpVector.length() > 12.0f) {
			return AIHelpers::createMoveActionToBall(*mPlayer);
		} else {
			Vector3 jumpVelocity = MatchHelpers::playerJumpVelocity(*mPlayer, jumpVector);
			if(jumpVelocity.null())
				return AIHelpers::createMoveActionToBall(*mPlayer);

			float ballFlyTime = (intersectionPoint - ballpos).length() / b->getSpeed();
			float myFlyTime = jumpVector.length() / jumpVelocity.length();
			if(myFlyTime + time > ballFlyTime) {
				return boost::shared_ptr<PlayerAction>(new JumpToPA(jumpVector));
			} else {
				return AIHelpers::createMoveActionToBall(*mPlayer);
			}
		}
	}
}


