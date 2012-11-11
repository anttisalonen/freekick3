#include "match/PlayerActions.h"
#include "match/MatchHelpers.h"

#include "match/ai/AIPlayStates.h"
#include "match/ai/AIHelpers.h"
#include "match/ai/AIActions.h"

AIOffensiveState::AIOffensiveState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

boost::shared_ptr<PlayerAction> AIOffensiveState::actOffBall(double time)
{
	bool oppAtt = AIHelpers::opponentAttacking(*mPlayer);
	if(mPlayer->getPlayerPosition() != Soccer::PlayerPosition::Forward &&
			oppAtt) {
		return mPlayController->switchState(boost::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
	}
	else if(oppAtt && mPlayer->getMatch()->getPlayState() != PlayState::InPlay) {
		return boost::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		if(mPlayer->getMatch()->getPlayState() == PlayState::InPlay) {
			auto fetchAction = boost::shared_ptr<AIAction>(new AIFetchBallAction(mPlayer));
			if(fetchAction->getScore() > 0.2f) {
				mDescription = fetchAction->getDescription();
				return fetchAction->getAction();
			}
		}

		mDescription = std::string("Supporting");
		return AIHelpers::createMoveActionTo(*mPlayer,
				AIHelpers::getShotPosition(*mPlayer));
	}
}


