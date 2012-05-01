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
			std::shared_ptr<PlayerController> newstate(std::shared_ptr<PlayerController>(new AIKickBallState(mPlayer, mMainAI)));
			mMainAI->setNewPlayState(newstate);
			return newstate->act(time);
		}
		else {
			return AIHelpers::createMoveActionTo(*mPlayer,
					mPlayer->getMatch()->getBall()->getPosition());
		}
	}
	else {
		/* TODO */
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
}


