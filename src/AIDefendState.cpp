#include "AIPlayStates.h"
#include "PlayerActions.h"
#include "PlayerAIController.h"
#include "MatchHelpers.h"
#include "AIHelpers.h"

AIDefendState::AIDefendState(Player* p, PlayerAIController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIDefendState::act(double time)
{
	if(mPlayer->getTeam()->getPlayerNearestToBall() == mPlayer) {
		if(MatchHelpers::canKickBall(*mPlayer)) {
			return switchState(std::shared_ptr<AIState>(new AIKickBallState(mPlayer, mMainAI)), time);
		}
		else {
			return AIHelpers::createMoveActionTo(*mPlayer,
					mPlayer->getMatch()->getBall()->getPosition());
		}
	}
	else {
		if(mPlayer->getTactics().mOffensive) {
			return switchState(std::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mMainAI)), time);
		}
		else {
			/* TODO */
			return std::shared_ptr<PlayerAction>(new IdlePA());
		}
	}
}


