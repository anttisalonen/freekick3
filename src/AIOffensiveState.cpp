#include "AIPlayStates.h"
#include "PlayerActions.h"
#include "MatchHelpers.h"
#include "AIHelpers.h"

AIOffensiveState::AIOffensiveState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIOffensiveState::actOnBall(double time)
{
	return mPlayController->switchState(std::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
}

std::shared_ptr<PlayerAction> AIOffensiveState::actNearBall(double time)
{
	return AIHelpers::createMoveActionTo(*mPlayer,
			mPlayer->getMatch()->getBall()->getPosition());
}

std::shared_ptr<PlayerAction> AIOffensiveState::actOffBall(double time)
{
	if(!mPlayer->getTactics().mOffensive && !MatchHelpers::myTeamInControl(*mPlayer)) {
		return mPlayController->switchState(std::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
	}
	else {
		/* TODO */
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
}


