#include "AIPlayStates.h"
#include "PlayerActions.h"
#include "MatchHelpers.h"
#include "PlayerAIController.h"

AIKickBallState::AIKickBallState(Player* p, AIPlayController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIKickBallState::actOnBall(double time)
{
	double passscore = 0.0;
	double dribblescore = 0.0;
	double shootscore = 0.0;
	Player* passplayer = nullptr;
	AbsVector3 dribbletarget;
	AbsVector3 shoottarget;

	passscore = getBestPassTarget(passplayer);
	dribblescore = getBestDribbleTarget(&dribbletarget);
	shootscore = getBestShootTarget(&shoottarget);
	mPlayController->setNewState(std::shared_ptr<AIState>(new AIOffensiveState(mPlayer, mPlayController)));
	if(passscore >= dribblescore && passscore >= shootscore) {
		return std::shared_ptr<PlayerAction>(new KickBallPA(passplayer->getPosition()));
	}
	else if(dribblescore >= passscore && dribblescore >= shootscore) {
		return std::shared_ptr<PlayerAction>(new KickBallPA(dribbletarget));
	}
	else {
		return std::shared_ptr<PlayerAction>(new KickBallPA(shoottarget));
	}

	return std::shared_ptr<PlayerAction>(new IdlePA());
}

std::shared_ptr<PlayerAction> AIKickBallState::actNearBall(double time)
{
	return mPlayController->switchState(std::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
}

std::shared_ptr<PlayerAction> AIKickBallState::actOffBall(double time)
{
	return switchState(std::shared_ptr<AIState>(new AIDefendState(mPlayer, mPlayController)), time);
}

double AIKickBallState::getBestPassTarget(Player* p)
{
	/* TODO */
	p = mPlayer;
	return 0.0;
}

double AIKickBallState::getBestDribbleTarget(AbsVector3* v)
{
	/* TODO */
	v->v.x = 0;
	v->v.y = 0;
	return 0.0;
}

double AIKickBallState::getBestShootTarget(AbsVector3* v)
{
	/* TODO */
	return 1.0;
}


