#include "PlayerActions.h"
#include "AIPlayStates.h"
#include "PlayerAIController.h"
#include "MatchHelpers.h"
#include "AIHelpers.h"

AIPlayController::AIPlayController(Player* p)
	: PlayerController(p)
{
	if(p->isGoalkeeper())
		mCurrentState = std::shared_ptr<AIState>(new AIGoalkeeperState(p, this));
	else
		mCurrentState = std::shared_ptr<AIState>(new AIDefendState(p, this));
}

std::shared_ptr<PlayerAction> AIPlayController::act(double time)
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

std::shared_ptr<PlayerAction> AIPlayController::switchState(std::shared_ptr<AIState> newstate, double time)
{
	mCurrentState = newstate;
	return act(time);
}

void AIPlayController::setNewState(std::shared_ptr<AIState> newstate)
{
	mCurrentState = newstate;
}

const std::string& AIPlayController::getDescription() const
{
	return mCurrentState->getDescription();
}

std::shared_ptr<PlayerAction> AIPlayController::actOnRestart(double time)
{
	if(MatchHelpers::myTeamInControl(*mPlayer)) {
		return mCurrentState->actOffBall(time);
	}
	else {
		AbsVector3 dir = MatchEntity::vectorFromTo(*mPlayer->getMatch()->getBall(),
				*mPlayer);
		float disttooppgoal = (mPlayer->getPosition().v - MatchHelpers::oppositeGoalPosition(*mPlayer).v).length();
		if(dir.v.length() < 9.15f || disttooppgoal < 25.0f) {
				return AIHelpers::createMoveActionTo(*mPlayer,
						AbsVector3(mPlayer->getPosition().v + dir.v));
		}
		else {
			return mCurrentState->actOffBall(time);
		}
	}
}

AIState::AIState(Player* p, AIPlayController* m)
	: mPlayer(p),
	mPlayController(m)
{
}

std::shared_ptr<PlayerAction> AIState::switchState(std::shared_ptr<AIState> newstate, double time)
{
	return mPlayController->switchState(newstate, time);
}

void AIState::setNewState(std::shared_ptr<AIState> newstate)
{
	mPlayController->setNewState(newstate);
}

const std::string& AIState::getDescription() const
{
	return mDescription;
}


