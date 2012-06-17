#include "match/ai/AIPlayStates.h"
#include "match/PlayerActions.h"
#include "match/MatchHelpers.h"
#include "match/ai/PlayerAIController.h"
#include "match/ai/AIActions.h"

AIKickBallState::AIKickBallState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

boost::shared_ptr<PlayerAction> AIKickBallState::actOnBall(double time)
{
	std::vector<boost::shared_ptr<AIAction>> actions;
	actions.push_back(boost::shared_ptr<AIAction>(new AILongPassAction(mPlayer)));
	actions.push_back(boost::shared_ptr<AIAction>(new AIPassAction(mPlayer)));
	actions.push_back(boost::shared_ptr<AIAction>(new AIShootAction(mPlayer)));
	actions.push_back(boost::shared_ptr<AIAction>(new AIClearAction(mPlayer)));
	actions.push_back(boost::shared_ptr<AIAction>(new AITackleAction(mPlayer)));

	if(mPlayer->getMatch()->getPlayState() == PlayState::InPlay)
		actions.push_back(boost::shared_ptr<AIAction>(new AIDribbleAction(mPlayer)));

	AIActionChooser actionchooser(actions, false);

	if(mPlayer->isGoalkeeper())
		mPlayController->setNewState(boost::shared_ptr<AIState>(new AIGoalkeeperState(mPlayer, mPlayController)));
	else
		mPlayController->setNewState(boost::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)));

	boost::shared_ptr<AIAction> best = actionchooser.getBestAction();
	mDescription = std::string("Defending - ") + best->getName();
	return best->getAction();
}

boost::shared_ptr<PlayerAction> AIKickBallState::actNearBall(double time)
{
	return mPlayController->switchState(boost::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
}

boost::shared_ptr<PlayerAction> AIKickBallState::actOffBall(double time)
{
	return switchState(boost::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
}

