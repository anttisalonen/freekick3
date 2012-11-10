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

boost::shared_ptr<PlayerAction> AIDefendState::actOffBall(double time)
{
	switch(mPlayer->getPlayerPosition()) {
		case Soccer::PlayerPosition::Goalkeeper:
			return mPlayController->switchState(boost::shared_ptr<AIState>(new AIGoalkeeperState(mPlayer, mPlayController)), time);

		case Soccer::PlayerPosition::Midfielder:
		case Soccer::PlayerPosition::Defender:
			{
				if(MatchHelpers::myTeamInControl(*mPlayer) &&
						(!playing(mPlayer->getMatch()->getPlayState()) ||
						 mPlayer->getPlayerPosition() == Soccer::PlayerPosition::Midfielder ||
						 mPlayer->getTactics().Offensive)) {
					return mPlayController->switchState(boost::shared_ptr<AIState>(new AIMidfielderState(mPlayer, mPlayController)), time);
				}

				std::vector<boost::shared_ptr<AIAction>> actions;
				if(mPlayer->getMatch()->getPlayState() == PlayState::InPlay) {
					actions.push_back(boost::shared_ptr<AIAction>(new AIFetchBallAction(mPlayer)));
				}
				actions.push_back(boost::shared_ptr<AIAction>(new AIBlockAction(mPlayer)));
				actions.push_back(boost::shared_ptr<AIAction>(new AIGuardAction(mPlayer)));
				actions.push_back(boost::shared_ptr<AIAction>(new AIBlockPassAction(mPlayer)));
				actions.push_back(boost::shared_ptr<AIAction>(new AITackleAction(mPlayer)));
				actions.push_back(boost::shared_ptr<AIAction>(new AIGuardAreaAction(mPlayer)));
				AIActionChooser actionchooser(actions, false);

				boost::shared_ptr<AIAction> best = actionchooser.getBestAction();
				mDescription = std::string("Defending - ") + best->getDescription();
				return best->getAction();
			}

		case Soccer::PlayerPosition::Forward:
			return mPlayController->switchState(boost::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)), time);
	}
	assert(0);
	throw std::runtime_error("AI: Unknown player position");
}

