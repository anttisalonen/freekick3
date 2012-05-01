#include "AIPlayStates.h"
#include "PlayerActions.h"
#include "MatchHelpers.h"
#include "PlayerAIController.h"

AIKickBallState::AIKickBallState(Player* p, PlayerAIController* m)
	: AIState(p, m)
{
}

std::shared_ptr<PlayerAction> AIKickBallState::act(double time)
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
	mMainAI->setNewPlayState(std::shared_ptr<PlayerController>(new AIOffensiveState(mPlayer, mMainAI)));
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


