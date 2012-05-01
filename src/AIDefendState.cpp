#include "AIPlayStates.h"
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
	return AIHelpers::createMoveActionTo(*mPlayer,
			mPlayer->getMatch()->getBall()->getPosition());
}

std::shared_ptr<PlayerAction> AIDefendState::actOffBall(double time)
{
	if(mPlayer->getTactics().mOffensive) {
		return mPlayController->switchState(std::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)), time);
	}
	else {
		/* TODO */
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
}

