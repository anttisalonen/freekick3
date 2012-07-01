#include "match/PlayerActions.h"
#include "match/ai/AIPlayStates.h"
#include "match/ai/PlayerAIController.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

AIPlayController::AIPlayController(Player* p)
	: PlayerController(p)
{
	if(p->isGoalkeeper())
		mCurrentState = boost::shared_ptr<AIState>(new AIGoalkeeperState(p, this));
	else
		mCurrentState = boost::shared_ptr<AIState>(new AIDefendState(p, this));
}

boost::shared_ptr<PlayerAction> AIPlayController::act(double time)
{
	if(mPlayer->getMatch()->getBall()->grabbed()) {
		if(mPlayer->getMatch()->getBall()->getGrabber() == mPlayer) {
			return mCurrentState->actOnBall(time);
		}
		else {
			return actOnRestart(time);
		}
	}
	else {
		if(mPlayer->getTeam()->getPlayerNearestToBall() == mPlayer) {
			if(MatchHelpers::canKickBall(*mPlayer)) {
				return mCurrentState->actOnBall(time);
			}
			else {
				return mCurrentState->actNearBall(time);
			}
		}
		else {
			return mCurrentState->actOffBall(time);
		}
	}
}

boost::shared_ptr<PlayerAction> AIPlayController::switchState(boost::shared_ptr<AIState> newstate, double time)
{
	mCurrentState = newstate;
	return act(time);
}

void AIPlayController::setNewState(boost::shared_ptr<AIState> newstate)
{
	mCurrentState = newstate;
}

const std::string& AIPlayController::getDescription() const
{
	return mCurrentState->getDescription();
}

boost::shared_ptr<PlayerAction> AIPlayController::actOnRestart(double time)
{
	if(MatchHelpers::myTeamInControl(*mPlayer)) {
		return mCurrentState->actOffBall(time);
	}
	else {
		AbsVector3 dir = MatchEntity::vectorFromTo(*mPlayer->getMatch()->getBall(),
				*mPlayer);
		float disttooppgoal = (mPlayer->getPosition().v - MatchHelpers::oppositeGoalPosition(*mPlayer).v).length();
		if(dir.v.length() < 9.5f || disttooppgoal < 30.0f) {
			// NOTE: max limit of disttooppgoal must not exceed half of the pitch width.
			// In that case a player may stay outside the pitch and the match would hang.
			return AIHelpers::createMoveActionTo(*mPlayer,
					AbsVector3(mPlayer->getPosition().v + dir.v));
		}
		else {
			return mCurrentState->actOffBall(time);
		}
	}
}

void AIPlayController::matchHalfChanged(MatchHalf m)
{
	mCurrentState->matchHalfChanged(m);
}

AIState::AIState(Player* p, AIPlayController* m)
	: mPlayer(p),
	mPlayController(m)
{
}

boost::shared_ptr<PlayerAction> AIState::actOnBall(double time)
{
	mDescription = std::string("Preparing kick");
	return mPlayController->switchState(boost::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
}

boost::shared_ptr<PlayerAction> AIState::actNearBall(double time)
{
	mDescription = std::string("Fetching");
	return AIHelpers::createMoveActionToBall(*mPlayer);
}

boost::shared_ptr<PlayerAction> AIState::switchState(boost::shared_ptr<AIState> newstate, double time)
{
	return mPlayController->switchState(newstate, time);
}

void AIState::setNewState(boost::shared_ptr<AIState> newstate)
{
	mPlayController->setNewState(newstate);
}

const std::string& AIState::getDescription() const
{
	return mDescription;
}


