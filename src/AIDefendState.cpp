#include "AIPlayStates.h"
#include "AIActions.h"
#include "PlayerActions.h"
#include "PlayerAIController.h"
#include "MatchHelpers.h"
#include "AIHelpers.h"

AIDefendState::AIDefendState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIDefendState::actOnBall(double time)
{
	return mPlayController->switchState(std::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
}

std::shared_ptr<PlayerAction> AIDefendState::actNearBall(double time)
{
	return AIHelpers::createMoveActionToBall(*mPlayer);
}

std::shared_ptr<PlayerAction> AIDefendState::actOffBall(double time)
{
	if(mPlayer->getTactics().mOffensive) {
		return mPlayController->switchState(std::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)), time);
	}
	else if(mPlayer->isGoalkeeper()) {
		return mPlayController->switchState(std::shared_ptr<AIState>(new AIGoalkeeperState(mPlayer, mPlayController)), time);
	}
	else {
		std::vector<std::shared_ptr<AIAction>> actions;
		actions.push_back(std::shared_ptr<AIAction>(new AIFetchBallAction(mPlayer)));
		actions.push_back(std::shared_ptr<AIAction>(new AIBlockAction(mPlayer)));
		actions.push_back(std::shared_ptr<AIAction>(new AIGuardAction(mPlayer)));
		actions.push_back(std::shared_ptr<AIAction>(new AIBlockPassAction(mPlayer)));
		AIActionChooser actionchooser(actions);

		std::shared_ptr<AIAction> best = actionchooser.getBestAction();
		mDescription = std::string("Defending - ") + best->getName();
		return best->getAction();
	}
}

