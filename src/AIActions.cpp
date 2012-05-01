#include <assert.h>

#include "AIActions.h"
#include "MatchHelpers.h"

AIActionChooser::AIActionChooser(const std::vector<std::shared_ptr<AIAction>>& actions)
	: mBestAction(nullptr)
{
	double bestscore = -1.0;

	assert(actions.size() > 0);

	for(auto a : actions) {
		double thisscore = a->getScore();
		if(thisscore >= bestscore) {
			bestscore = thisscore;
			mBestAction = a;
		}
	}
}

std::shared_ptr<AIAction> AIActionChooser::getBestAction()
{
	return mBestAction;
}

AIAction::AIAction(const char* name, const Player* p)
	: mName(name),
	mPlayer(p),
	mScore(-1.0),
	mAction(nullptr)
{
}

std::shared_ptr<PlayerAction> AIAction::getAction() const
{
	return mAction;
}

double AIAction::getScore() const
{
	return mScore;
}

const char* AIAction::getName() const
{
	return mName;
}

AINullAction::AINullAction(const Player* p)
	: AIAction(mActionName, p)
{
	/* TODO */
	mScore = 0.0;
	mAction = std::shared_ptr<PlayerAction>(new IdlePA());
}

const char* AINullAction::mActionName = "Null";

AIShootAction::AIShootAction(const Player* p)
	: AIAction(mActionName, p)
{
	/* TODO */
	mScore = 1.0;
	mAction = std::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p)));
}

const char* AIShootAction::mActionName = "Shoot";

AIDribbleAction::AIDribbleAction(const Player* p)
	: AIAction(mActionName, p)
{
	/* TODO */
	mScore = 0.0;
	mAction = std::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p)));
}

const char* AIDribbleAction::mActionName = "Dribble";

AIPassAction::AIPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	/* TODO */
	mScore = 1.0;
	mAction = std::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p)));
}

const char* AIPassAction::mActionName = "Pass";


