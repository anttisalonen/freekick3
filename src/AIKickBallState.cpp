#include "AIPlayStates.h"
#include "PlayerActions.h"
#include "MatchHelpers.h"
#include "PlayerAIController.h"
#include "AIActions.h"

AIKickBallState::AIKickBallState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIKickBallState::actOnBall(double time)
{
	std::vector<std::shared_ptr<AIAction>> actions;
	actions.push_back(std::shared_ptr<AIAction>(new AIPassAction(mPlayer)));
	actions.push_back(std::shared_ptr<AIAction>(new AIShootAction(mPlayer)));

	if(mPlayer->getMatch()->getPlayState() == PlayState::InPlay)
		actions.push_back(std::shared_ptr<AIAction>(new AIDribbleAction(mPlayer)));

	AIActionChooser actionchooser(actions);

	if(mPlayer->isGoalkeeper())
		mPlayController->setNewState(std::shared_ptr<AIState>(new AIGoalkeeperState(mPlayer, mPlayController)));
	else
		mPlayController->setNewState(std::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)));

	std::shared_ptr<AIAction> best = actionchooser.getBestAction();
	mDescription = std::string("Defending - ") + best->getName();
	return best->getAction();
}

std::shared_ptr<PlayerAction> AIKickBallState::actNearBall(double time)
{
	return mPlayController->switchState(std::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
}

std::shared_ptr<PlayerAction> AIKickBallState::actOffBall(double time)
{
	return switchState(std::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
}

