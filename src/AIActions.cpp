#include <assert.h>
#include <iostream>
#include <algorithm>

#include "AIActions.h"
#include "AIHelpers.h"
#include "MatchHelpers.h"
#include "Math.h"

AIActionChooser::AIActionChooser(const std::vector<std::shared_ptr<AIAction>>& actions)
	: mBestAction(nullptr)
{
	double bestscore = -1.0;

	assert(actions.size() > 0);

	for(auto a : actions) {
		double thisscore = std::max(0.0, a->getScore());
		// printf("Action: %10s: %3.3f\n", a->getName(), thisscore);
		if(thisscore >= bestscore) {
			bestscore = thisscore;
			mBestAction = a;
		}
	}
	// printf(" => %s\n", mBestAction->getName());
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
	const float maxOppDist = 2.0f;

	for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
		float dist = Math::pointToLineDistance(p->getPosition().v,
				p->getPosition().v + shoottarget.v,
				op->getPosition().v);
		if(dist < maxOppDist) {
			mScore -= 0.3f * ((maxOppDist - dist) / maxOppDist);
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
		double dist = MatchEntity::distanceBetween(*p, *sp);
		if(dist < 5.0)
			continue;
		if(dist > 30.0)
			continue;
		double thisscore = 1.0f;
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
					thisscore -= 0.5f * ((5.0 - dist) / 5.0);
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

AIFetchBallAction::AIFetchBallAction(const Player* p)
	: AIAction(mActionName, p)
{
	float maxdist = 20.0f;
	float dist = MatchEntity::distanceBetween(*p,
			*p->getMatch()->getBall());
	mScore = 0.5f * std::max(0.0f, (maxdist - dist) / maxdist);
	mAction = AIHelpers::createMoveActionTo(*p, p->getMatch()->getBall()->getPosition());
}

const char* AIFetchBallAction::mActionName = "Fetch";

AIGuardAction::AIGuardAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move between opposing supporting player and own goal
	AbsVector3 owngoal = MatchHelpers::ownGoalPosition(*p);
	float highestdangerousness = -1.0f;
	AbsVector3 tgtpos(p->getPosition());
	for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
		float dangerousness = MatchHelpers::getOpposingTeam(*p)->getSupportingPositionScoreAt(op->getPosition());
		for(auto pl : MatchHelpers::getOwnPlayers(*p)) {
			if(&*pl == p)
				continue;
			float disttoown = Math::pointToLineDistance(op->getPosition().v,
					owngoal.v,
					pl->getPosition().v);
			if(disttoown < 1.0f) {
				dangerousness = 0.01f;
			}
		}
		if(dangerousness > highestdangerousness) {
			highestdangerousness = dangerousness;
			tgtpos = (op->getPosition().v + owngoal.v) * 0.5f;
		}
	}
	mScore = highestdangerousness;
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos);
}

const char* AIGuardAction::mActionName = "Guard";

AIBlockAction::AIBlockAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move between the opposing player holding the ball
	// and own goal
	AbsVector3 owngoal = MatchHelpers::ownGoalPosition(*p);
	const Player* op = MatchHelpers::nearestOppositePlayerToBall(*p->getTeam());
	float disttogoal = (owngoal.v - op->getPosition().v).length();
	mScore = std::min(1.0f, (100.0f - disttogoal) / 50.0f);
	for(auto pl : MatchHelpers::getOwnPlayers(*p)) {
		if(&*pl == p || pl->isGoalkeeper())
			continue;
		float disttoown = Math::pointToLineDistance(op->getPosition().v,
				owngoal.v,
				pl->getPosition().v);
		if(disttoown < 1.0f) {
			mScore = 0.02f;
			break;
		}
	}
	mAction = AIHelpers::createMoveActionTo(*p, AbsVector3((op->getPosition().v + owngoal.v) * 0.5f));
}

const char* AIBlockAction::mActionName = "Block";

AIBlockPassAction::AIBlockPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move between opposing supporting player and opponent holding the ball
	AbsVector3 owngoal = MatchHelpers::ownGoalPosition(*p);
	const Player* op = MatchHelpers::nearestOppositePlayerToBall(*p->getTeam());
	AbsVector3 bestpos(p->getPosition());
	mScore = -1.0f;
	for(auto pl : MatchHelpers::getOpposingPlayers(*p)) {
		if(op == &*pl || pl->isGoalkeeper())
			continue;
		float disttogoal = (owngoal.v - pl->getPosition().v).length();
		if(disttogoal > 50.0f)
			continue;
		float thisscore = (50.0f - disttogoal) / 50.0f;
		if(thisscore > mScore) {
			mScore = thisscore;
			bestpos = pl->getPosition();
		}
	}
	mAction = AIHelpers::createMoveActionTo(*p, AbsVector3((p->getPosition().v + bestpos.v) * 0.5f));
}

const char* AIBlockPassAction::mActionName = "Block pass";


