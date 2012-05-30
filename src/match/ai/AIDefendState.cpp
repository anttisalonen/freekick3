#include <stdexcept>

#include "match/ai/AIPlayStates.h"
#include "match/ai/AIActions.h"
#include "match/PlayerActions.h"
#include "match/ai/PlayerAIController.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

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
	switch(mPlayer->getPlayerPosition()) {
		case Soccer::PlayerPosition::Goalkeeper:
			return mPlayController->switchState(std::shared_ptr<AIState>(new AIGoalkeeperState(mPlayer, mPlayController)), time);

		case Soccer::PlayerPosition::Defender:
			{
				std::vector<std::shared_ptr<AIAction>> actions;
				actions.push_back(std::shared_ptr<AIAction>(new AIFetchBallAction(mPlayer)));
				actions.push_back(std::shared_ptr<AIAction>(new AIBlockAction(mPlayer)));
				actions.push_back(std::shared_ptr<AIAction>(new AIGuardAction(mPlayer)));
				actions.push_back(std::shared_ptr<AIAction>(new AIBlockPassAction(mPlayer)));
				actions.push_back(std::shared_ptr<AIAction>(new AIGuardAreaAction(mPlayer)));
				AIActionChooser actionchooser(actions, false);

				std::shared_ptr<AIAction> best = actionchooser.getBestAction();
				mDescription = std::string("Defending - ") + best->getDescription();
				return best->getAction();
			}

		case Soccer::PlayerPosition::Midfielder:
			return mPlayController->switchState(std::shared_ptr<AIState>(new AIMidfielderState(mPlayer, mPlayController)), time);

		case Soccer::PlayerPosition::Forward:
			return mPlayController->switchState(std::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)), time);
	}
	assert(0);
	throw std::runtime_error("AI: Unknown player position");
}

