#include "match/ai/AIPlayStates.h"
#include "match/PlayerActions.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

AIMidfielderState::AIMidfielderState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

boost::shared_ptr<PlayerAction> AIMidfielderState::actOffBall(double time)
{
	if(mPlayer->getPlayerPosition() == Soccer::PlayerPosition::Defender &&
			!MatchHelpers::myTeamInControl(*mPlayer)) {
		return mPlayController->switchState(boost::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
	}

	if(MatchHelpers::myTeamInControl(*mPlayer) &&
			(mPlayer->getPlayerPosition() == Soccer::PlayerPosition::Midfielder ||
			 mPlayer->getPlayerPosition() == Soccer::PlayerPosition::Forward) &&
			(mPlayer->getMatch()->getPlayState() == PlayState::OutDirectFreekick ||
			 mPlayer->getMatch()->getPlayState() == PlayState::OutPenaltykick ||
			 mPlayer->getMatch()->getPlayState() == PlayState::OutCornerkick ||
			 mPlayer->getTactics().Offensive)) {
		return mPlayController->switchState(boost::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)), time);
	}

	if(!MatchHelpers::myTeamInControl(*mPlayer) &&
			mPlayer->getPlayerPosition() == Soccer::PlayerPosition::Midfielder &&
			(mPlayer->getMatch()->getPlayState() == PlayState::OutDirectFreekick ||
			 mPlayer->getMatch()->getPlayState() == PlayState::OutPenaltykick ||
			 mPlayer->getMatch()->getPlayState() == PlayState::OutCornerkick) &&
			 !mPlayer->getTactics().Offensive) {
		return mPlayController->switchState(boost::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
	}

	if(!MatchHelpers::myTeamInControl(*mPlayer) && mPlayer->getMatch()->getPlayState() != PlayState::InPlay) {
		return boost::shared_ptr<PlayerAction>(new IdlePA());
	}
	Common::Vector3 v = AIHelpers::getPassPosition(*mPlayer);
	std::stringstream ss;
	char buf[128];
	sprintf(buf, "Midfield %d %d", (int)v.x, (int)v.y);
	mDescription = std::string(buf);
	return AIHelpers::createMoveActionTo(*mPlayer, v);
}


