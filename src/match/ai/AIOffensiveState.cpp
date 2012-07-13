#include "match/ai/AIPlayStates.h"
#include "match/PlayerActions.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

AIOffensiveState::AIOffensiveState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

boost::shared_ptr<PlayerAction> AIOffensiveState::actOffBall(double time)
{
	if(mPlayer->getPlayerPosition() != Soccer::PlayerPosition::Forward &&
			!MatchHelpers::myTeamInControl(*mPlayer)) {
		return mPlayController->switchState(boost::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
	}
	else if(!MatchHelpers::myTeamInControl(*mPlayer) && mPlayer->getMatch()->getPlayState() != PlayState::InPlay) {
		return boost::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		mDescription = std::string("Supporting");
		return AIHelpers::createMoveActionTo(*mPlayer,
				AIHelpers::getShotPosition(*mPlayer));
	}
}


