#include "match/PlayerActions.h"
#include "match/ai/AIPlayStates.h"
#include "match/ai/PlayerAIController.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

using Common::Vector3;

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
		bool nearest = MatchHelpers::nearestOwnPlayerToBall(*mPlayer->getTeam()) == mPlayer;
		bool cankick = MatchHelpers::canKickBall(*mPlayer);
		if(cankick) {
			return mCurrentState->actOnBall(time);
		}
		else if(nearest) {
			return mCurrentState->actNearBall(time);
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
	// goalkeeper stays on the goal line for the penalty kick
	if(!MatchHelpers::myTeamInControl(*mPlayer) &&
			mPlayer->getMatch()->getPlayState() == PlayState::OutPenaltykick &&
			mPlayer->isGoalkeeper()) {
		return AIHelpers::createMoveActionTo(*mPlayer,
				MatchHelpers::ownGoalPosition(*mPlayer));
	}

	// players with shirt numbers 8-11 build a wall
	if(!MatchHelpers::myTeamInControl(*mPlayer) &&
			mPlayer->getMatch()->getPlayState() == PlayState::OutDirectFreekick &&
			!mPlayer->isGoalkeeper() && mPlayer->getShirtNumber() >= 8) {
		Vector3 ballpos = mPlayer->getMatch()->getBall()->getPosition();
		Vector3 goalpos = MatchHelpers::ownGoalPosition(*mPlayer);
		if((ballpos - goalpos).length() < 35.0f) {
			Vector3 mypos = (goalpos - ballpos).normalized() * 9.50f;

			Vector3 perp;
			perp.x = -mypos.y;
			perp.y = mypos.x;
			perp.normalize();
			float dist = 0.8f * (mPlayer->getShirtNumber() - 10);

			mypos += perp * dist;

			return AIHelpers::createMoveActionTo(*mPlayer,
					Vector3(ballpos + mypos));
		}
	}

	// if we're blocking the game, move away from the ball
	if(MatchHelpers::playerBlockingRestart(*mPlayer)) {
		Vector3 dir = MatchEntity::vectorFromTo(*mPlayer->getMatch()->getBall(),
				*mPlayer);

		if(dir.null() || mPlayer->getMatch()->getPlayState() == PlayState::OutPenaltykick) {
			dir = mPlayer->getMatch()->getBall()->getPosition();
			dir *= -1.0f;
		}
		dir.normalize();

		mCurrentState->blockedMatch();

		return AIHelpers::createMoveActionTo(*mPlayer,
				Vector3(mPlayer->getPosition() + dir));
	}
	else {
		if(mCurrentState->checkBlockedMatchTimer(time))
			return boost::shared_ptr<PlayerAction>(new IdlePA());
		else
			return mCurrentState->actOffBall(time);
	}
}

void AIPlayController::matchHalfChanged(MatchHalf m)
{
	mCurrentState->matchHalfChanged(m);
}

AIState::AIState(Player* p, AIPlayController* m)
	: mPlayer(p),
	mPlayController(m),
	mBlockedMatchTimer(2.0f)
{
}

bool AIState::checkBlockedMatchTimer(double time)
{
	mBlockedMatchTimer.doCountdown(time);
	mBlockedMatchTimer.check();
	return mBlockedMatchTimer.running();
}

void AIState::blockedMatch()
{
	mBlockedMatchTimer.rewind();
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


