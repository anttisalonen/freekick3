#include "match/ai/AIPlayStates.h"
#include "match/PlayerActions.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

AIMidfielderState::AIMidfielderState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIMidfielderState::actOnBall(double time)
{
	return mPlayController->switchState(std::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
}

std::shared_ptr<PlayerAction> AIMidfielderState::actNearBall(double time)
{
	mDescription = std::string("Midfield - fetching");
	return AIHelpers::createMoveActionToBall(*mPlayer);
}

std::shared_ptr<PlayerAction> AIMidfielderState::actOffBall(double time)
{
	mDescription = std::string("Midfield");
	return AIHelpers::createMoveActionTo(*mPlayer,
			AIHelpers::getPassPosition(*mPlayer));
}


