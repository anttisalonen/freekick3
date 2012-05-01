#include <assert.h>
#include <iostream>
#include <algorithm>

#include "AIActions.h"
#include "MatchHelpers.h"
#include "Math.h"

AIActionChooser::AIActionChooser(const std::vector<std::shared_ptr<AIAction>>& actions)
	: mBestAction(nullptr)
{
	double bestscore = -1.0;

	assert(actions.size() > 0);

	for(auto a : actions) {
		double thisscore = a->getScore();
		printf("Action: %10s - %3.3f\n", a->getName(), a->getScore());
		if(thisscore >= bestscore) {
			bestscore = thisscore;
			mBestAction = a;
		}
	}
	printf(" => %s\n", mBestAction->getName());
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
	assert(mAction);
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
	mScore = 0.0;
	mAction = std::shared_ptr<PlayerAction>(new IdlePA());
}

const char* AINullAction::mActionName = "Null";

AIShootAction::AIShootAction(const Player* p)
	: AIAction(mActionName, p)
{
	AbsVector3 shoottarget = MatchHelpers::oppositeGoalPosition(*p);
	mAction = std::shared_ptr<PlayerAction>(new KickBallPA(shoottarget, true));

	AbsVector3 vec(MatchHelpers::oppositePenaltySpotPosition(*p));
	vec.v -= p->getPosition().v;

	mScore = std::max(0.0f, 1.0f - vec.v.length() * 0.03f);
	float maxOppDist = shoottarget.v.length() * 0.5f;

	for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
		float dist = Math::pointToLineDistance(p->getPosition().v,
				p->getPosition().v + shoottarget.v,
				op->getPosition().v);
		if(dist < maxOppDist) {
			mScore -= (maxOppDist - dist) / maxOppDist;
			if(mScore <= 0.0)
				return;
		}
	}
}

const char* AIShootAction::mActionName = "Shoot";

AIDribbleAction::AIDribbleAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = 1.0;
	AbsVector3 vec;
	if(MatchHelpers::attacksUp(*p))
		vec.v.y = 1.0;
	else
		vec.v.y = -1.0;
	vec.v *= 0.35;
	mAction = std::shared_ptr<PlayerAction>(new KickBallPA(vec));
	for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
		float dist = Math::pointToLineDistance(p->getPosition().v,
				p->getPosition().v + vec.v,
				op->getPosition().v);
		if(dist < 8.0) {
			mScore -= (8.0 - dist) / 8.0;
			if(mScore <= 0.0)
				return;
		}
	}
}

const char* AIDribbleAction::mActionName = "Dribble";

AIPassAction::AIPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = -1.0;
	AbsVector3 tgt;
	mAction = std::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p), true));
	for(auto sp : MatchHelpers::getOwnPlayers(*p)) {
		if(&*sp == p)
			continue;
		double thisscore;
		double dist = MatchEntity::distanceBetween(*p, *sp);
		if(dist < 5.0)
			continue;
		if(dist > 30.0)
			continue;
		thisscore = (dist - 5.0) / 25.0;
		double progdist = ((sp->getPosition().v - p->getPosition().v).normalized().y + 1.0) * 0.5;
		if(!MatchHelpers::attacksUp(*p))
			progdist = -progdist;
		thisscore *= progdist;
		if(thisscore > mScore) {
			for(auto op : MatchHelpers::getOpposingPlayers(*sp)) {
				float dist = Math::pointToLineDistance(p->getPosition().v,
						sp->getPosition().v,
						op->getPosition().v);
				if(dist < 5.0) {
					thisscore -= (5.0 - dist) / 5.0;
				}
				if(thisscore > mScore) {
					mScore = thisscore;
					tgt = sp->getPosition();
				}
			}
		}
	}
	if(mScore >= 0) {
		mAction = std::shared_ptr<PlayerAction>(new KickBallPA(tgt, true));
	}
}

const char* AIPassAction::mActionName = "Pass";


