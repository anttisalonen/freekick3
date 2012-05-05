#include "Math.h"
#include "AIHelpers.h"
#include "MatchHelpers.h"
#include "AIPlayStates.h"
#include "PlayerActions.h"

AIGoalkeeperState::AIGoalkeeperState(Player* p, AIPlayController* m)
	: AIState(p, m),
	mHoldBallTimer(1.0f)
{
	if(MatchHelpers::attacksUp(*p)) {
		mPivotPoint = p->getMatch()->convertRelativeToAbsoluteVector(RelVector3(0, -1.2, 0));
	}
	else {
		mPivotPoint = p->getMatch()->convertRelativeToAbsoluteVector(RelVector3(0, 1.2, 0));
	}
	mDistanceFromPivot = p->getMatch()->getPitchHeight() * 0.15f;
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::actOnBall(double time)
{
	if(mPlayer->getMatch()->getBall()->grabbed() && mPlayer->getMatch()->getBall()->getGrabber() == mPlayer) {
		mHoldBallTimer.doCountdown(time);
		if(mHoldBallTimer.check()) {
			return mPlayController->switchState(std::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
		}
		else {
			return std::shared_ptr<PlayerAction>(new IdlePA());
		}
	}
	else {
		if(MatchHelpers::myTeamInControl(*mPlayer)) {
			return mPlayController->switchState(std::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
		}
		else {
			mHoldBallTimer.rewind();
			return std::shared_ptr<PlayerAction>(new GrabBallPA());
		}
	}
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::actNearBall(double time)
{
	return actOffBall(time);
}

std::shared_ptr<PlayerAction> AIGoalkeeperState::actOffBall(double time)
{
	AbsVector3 ballpos = mPlayer->getMatch()->getBall()->getPosition();
	AbsVector3 diffvec = ballpos.v - mPivotPoint.v;
	AbsVector3 vectoball = MatchEntity::vectorFromTo(*mPlayer, *mPlayer->getMatch()->getBall());
	if(vectoball.v.length() < 10.0f) {
		AbsVector3 vectogoal = MatchHelpers::ownGoalPosition(*mPlayer).v - mPlayer->getPosition().v;
		if(vectogoal.v.length() < 10.0f) {
			return AIHelpers::createMoveActionTo(*mPlayer, ballpos);
		}
	}
	AbsVector3 point = mPivotPoint.v + diffvec.v.normalized() * mDistanceFromPivot;
	point.v.x = clamp(-3.0f, point.v.x, 3.0f);
	if(mPivotPoint.v.y > 0)
		point.v.y = std::min(mPlayer->getMatch()->getPitchHeight() * 0.5f, point.v.y);
	else
		point.v.y = std::max(mPlayer->getMatch()->getPitchHeight() * -0.5f, point.v.y);

	return AIHelpers::createMoveActionTo(*mPlayer, point);
}

