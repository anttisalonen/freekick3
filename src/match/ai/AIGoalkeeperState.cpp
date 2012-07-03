#include "common/Math.h"

#include "match/ai/AIHelpers.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIPlayStates.h"
#include "match/PlayerActions.h"

AIGoalkeeperState::AIGoalkeeperState(Player* p, AIPlayController* m)
	: AIState(p, m),
	mHoldBallTimer(1.0f)
{
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
	if((MatchHelpers::ownGoalPosition(*mPlayer).v - mPlayer->getMatch()->getBall()->getPosition().v).length() < 5.0f &&
			mPlayer->getMatch()->getBall()->getVelocity().v.length() < 5.0f) {
		return AIHelpers::createMoveActionToBall(*mPlayer);
	}
	else if(MatchEntity::distanceBetween(*mPlayer, *mPlayer->getMatch()->getBall()) < 0.1f) {
		return actOnBall(time);
	}
	else {
		return actOffBall(time);
	}
}

boost::shared_ptr<PlayerAction> AIGoalkeeperState::actOffBall(double time)
{
	const Ball* ball = mPlayer->getMatch()->getBall();
	AbsVector3 ballpos = ball->getPosition();
	Vector3 futureballpos = ballpos.v + ball->getVelocity().v * 1.0f;
	Vector3 goalmiddlepoint = MatchHelpers::ownGoalPosition(*mPlayer).v;
	static const float gkdisttogoal = 1.0f;
	if(MatchHelpers::attacksUp(*mPlayer))
		goalmiddlepoint.y += gkdisttogoal;
	else
		goalmiddlepoint.y -= gkdisttogoal;

	float balltowardsgoal = Common::Math::pointToLineDistance(ballpos.v, futureballpos, goalmiddlepoint);
	if(Common::signum(ball->getVelocity().v.y) == Common::signum(goalmiddlepoint.y) && balltowardsgoal < GOAL_WIDTH_2) {
		Vector3 tgtpos = Common::Math::lineLineIntersection2D(ballpos.v,
				futureballpos,
				Vector3(-GOAL_WIDTH_2, goalmiddlepoint.y, 0),
				Vector3(GOAL_WIDTH_2, goalmiddlepoint.y, 0));
		// tgtpos may be null when the ball doesn't move
		if(!tgtpos.null()) {
			float timetogoal = (ballpos.v - goalmiddlepoint).length() / ball->getVelocity().v.length();
			if(timetogoal < 0.5f && futureballpos.z > 2.0f &&
					(tgtpos - mPlayer->getPosition().v).length() < 1.0f) {
				// jump
				tgtpos -= mPlayer->getPosition().v;
				tgtpos.z = fabs(futureballpos.z);
				return boost::shared_ptr<PlayerAction>(new JumpToPA(AbsVector3(tgtpos)));
			}
			else {
				return AIHelpers::createMoveActionTo(*mPlayer, AbsVector3(tgtpos));
			}
		}
	}

	AbsVector3 diffvec = ballpos.v - mPivotPoint.v;
	AbsVector3 vectoball = MatchEntity::vectorFromTo(*mPlayer, *mPlayer->getMatch()->getBall());
	if(vectoball.v.length() < 10.0f) {
		AbsVector3 vectogoal = MatchHelpers::ownGoalPosition(*mPlayer).v - mPlayer->getPosition().v;
		if(vectogoal.v.length() < 10.0f) {
			return AIHelpers::createMoveActionTo(*mPlayer, ballpos);
		}
	}
	AbsVector3 point = mPivotPoint.v + diffvec.v.normalized() * mDistanceFromPivot;
	point.v.x = Common::clamp(-3.0f, point.v.x, 3.0f);
	if(mPivotPoint.v.y > 0)
		point.v.y = std::min(mPlayer->getMatch()->getPitchHeight() * 0.5f, point.v.y);
	else
		point.v.y = std::max(mPlayer->getMatch()->getPitchHeight() * -0.5f, point.v.y);

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


