#include <assert.h>
#include <iostream>
#include <algorithm>

#include "common/Math.h"

#include "match/ai/AIActions.h"
#include "match/ai/AIHelpers.h"
#include "match/MatchHelpers.h"

AIActionChooser::AIActionChooser(const std::vector<boost::shared_ptr<AIAction>>& actions,
		bool debug)
	: mBestAction(boost::shared_ptr<AIAction>())
{
	double bestscore = -1.0;

	assert(actions.size() > 0);

	for(auto a : actions) {
		double thisscore = a->getScore();
		// printf("Action: %10s: %3.3f\n", a->getName(), thisscore);
		if(thisscore >= bestscore) {
			bestscore = thisscore;
			mBestAction = a;
		}
	}
	// printf(" => %s\n", mBestAction->getName());
	if(debug || bestscore <= 0.0f) {
		printf("%sbest score is %3.3f for %s\n",
				debug ? "" : "Warning: ", bestscore, mBestAction->getName());
		for(auto a : actions) {
			double thisscore = a->getScore();
			printf("Action: %10s: %3.3f\n", a->getName(), thisscore);
		}
	}
}

boost::shared_ptr<AIAction> AIActionChooser::getBestAction()
{
	return mBestAction;
}

AIAction::AIAction(const char* name, const Player* p)
	: mName(name),
	mPlayer(p),
	mScore(-1.0),
	mAction(boost::shared_ptr<PlayerAction>())
{
}

boost::shared_ptr<PlayerAction> AIAction::getAction() const
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

std::string AIAction::getDescription() const
{
	return std::string(mName) + " " + mAction->getDescription();
}

AINullAction::AINullAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = 0.0;
	mAction = boost::shared_ptr<PlayerAction>(new IdlePA());
}

const char* AINullAction::mActionName = "Null";

AIShootAction::AIShootAction(const Player* p)
	: AIAction(mActionName, p)
{
	AbsVector3 shoottarget = MatchHelpers::oppositeGoalPosition(*p);
	AbsVector3 tgt = shoottarget;

	PlayState ps = p->getMatch()->getPlayState();
	if(ps == PlayState::OutThrowin ||
			ps == PlayState::OutKickoff ||
			ps == PlayState::OutGoalkick ||
			ps == PlayState::OutIndirectFreekick ||
			ps == PlayState::OutDroppedball) {
		mScore = -1.0f;
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(shoottarget, nullptr, true));
		return;
	}

	AbsVector3 vec(MatchHelpers::oppositePenaltySpotPosition(*p));
	vec.v -= p->getPosition().v;

	if((p->getPosition().v - shoottarget.v).length() < 6.0f) {
		mScore = 1.0f;
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(shoottarget, nullptr, true));
		return;
	}

	float defscore = std::max(0.0f, 1.0f - vec.v.length() * 0.02f);
	float maxscore = -1.0f;
	const float maxOppDist = 2.0f;

	std::vector<AbsVector3> shoottargets;

	shoottargets.push_back(shoottarget);
	shoottargets.push_back(AbsVector3(shoottarget.v + Vector3(GOAL_WIDTH_2 - 0.2f, 0, 0)));
	shoottargets.push_back(AbsVector3(shoottarget.v + Vector3(-GOAL_WIDTH_2 + 0.2f, 0, 0)));

	for(auto thistgt : shoottargets) {
		float thisscore = defscore;
		for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
			float dist = Common::Math::pointToLineDistance(p->getPosition().v,
					thistgt.v,
					op->getPosition().v);
			if(dist < maxOppDist) {
				/* TODO: this value is excellent for configuring whether the team
				 * tries to score from afar, or perfects the shot position, so
				 * have it depend on the team tactics. Setting this too high (> 0.2)
				 * may lead to excessive trying-to-kick-the-ball-through-opponents. */
				static const float coeff = 0.1f;
				thisscore -= coeff * ((maxOppDist - dist) / maxOppDist);
				if(thisscore <= 0.0) {
					thisscore = 0.0f;
					break;
				}
			}
		}
		thisscore = AIHelpers::checkKickSuccess(*mPlayer, AbsVector3(thistgt.v - mPlayer->getPosition().v),
							thisscore);
		if(thisscore > maxscore) {
			thistgt.v.z = (thistgt.v - p->getPosition().v).length() * 0.05f;
			maxscore = thisscore;
			tgt = thistgt;
		}
	}

	mScore = maxscore;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, nullptr, true));
}

const char* AIShootAction::mActionName = "Shoot";

AIClearAction::AIClearAction(const Player* p)
	: AIAction(mActionName, p)
{
	AbsVector3 tgt = MatchHelpers::oppositeGoalPosition(*p);
	float distToOwnGoal = (MatchHelpers::ownGoalPosition(*p).v - p->getMatch()->getBall()->getPosition().v).length();
	float distToOpposingPlayer = MatchEntity::distanceBetween(*p->getMatch()->getBall(),
				*MatchHelpers::nearestOppositePlayerToBall(*p->getTeam()));

	const float maxDist = 20.0f;

	if(distToOpposingPlayer < 5.0f)
		mScore = AIHelpers::scaledCoefficient(distToOwnGoal, maxDist);
	else
		mScore = -1.0f;

	if(mScore > 0.0f) {
		Vector3 vecToBall = p->getMatch()->getBall()->getPosition().v - p->getPosition().v;
		if(vecToBall.null()) {
			vecToBall = MatchHelpers::oppositeGoalPosition(*p).v - p->getPosition().v;
		}
		vecToBall.normalize();

		Vector3 kickvec1(vecToBall);
		Vector3 kickvec2(vecToBall);
		const float sr1 = sin(PI / 4.0f);
		const float cr1 = cos(PI / 4.0f);
		const float sr2 = sin(-PI / 4.0f);
		const float cr2 = cos(-PI / 4.0f);
		kickvec1.x = vecToBall.x * cr1 - vecToBall.y * sr1;
		kickvec1.y = vecToBall.x * sr1 + vecToBall.y * cr1;
		kickvec2.x = vecToBall.x * cr2 - vecToBall.y * sr2;
		kickvec2.y = vecToBall.x * sr2 + vecToBall.y * cr2;

		kickvec1 *= 50.0f;
		kickvec2 *= 50.0f;

		float dist1 = (MatchHelpers::ownGoalPosition(*p).v - kickvec1).length();
		float dist2 = (MatchHelpers::ownGoalPosition(*p).v - kickvec2).length();

		tgt = AbsVector3(dist1 > dist2 ? kickvec1 : kickvec2);
		tgt.v.z = tgt.v.length() * 0.8f;
		mScore = AIHelpers::checkKickSuccess(*mPlayer, tgt, mScore);
	}

	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, nullptr, false));
}

const char* AIClearAction::mActionName = "Clear";

AIDribbleAction::AIDribbleAction(const Player* p)
	: AIAction(mActionName, p)
{
	float bestscore = -1.0f;
	AbsVector3 bestvec;
	std::vector<AbsVector3> tgtvectors;
	const float dribblelen = 4.0f;
	for(int i = 0; i < 12; i++) {
		AbsVector3 vec;
		vec.v.x = dribblelen * sin(i * 2 * PI / 12.0f);
		vec.v.y = dribblelen * cos(i * 2 * PI / 12.0f);
		tgtvectors.push_back(vec);
	}
	for(auto vec : tgtvectors) {
		if(!MatchHelpers::onPitch(*p->getMatch(), AbsVector3(p->getPosition().v + vec.v.normalized() * 9.0f)))
			continue;
		float thisscore = 1.0f;
		for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
			float dist = Common::Math::pointToLineDistance(p->getPosition().v,
					p->getPosition().v + vec.v * 2.0f,
					op->getPosition().v);
			if(dist < 12.0) {
				thisscore -= (12.0 - dist) / 12.0;
				if(thisscore <= 0.0) {
					break;
				}
			}
		}
		if(MatchHelpers::attacksUp(*p))
			thisscore = thisscore * ((vec.v.y + dribblelen) / (2.0f * dribblelen));
		else
			thisscore = thisscore * ((vec.v.y - dribblelen) / (2.0f * dribblelen));

		thisscore = AIHelpers::checkKickSuccess(*mPlayer, vec, thisscore);

		if(thisscore > bestscore) {
			bestscore = thisscore;
			bestvec = AbsVector3(vec.v.normalized() * 0.3f);
		}
	}
	mScore = bestscore;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(bestvec));
}

const char* AIDribbleAction::mActionName = "Dribble";

AIPassAction::AIPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = -1.0;
	AbsVector3 tgt;
	AbsVector3 thistgt;
	Player* tgtPlayer = nullptr;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p),
				nullptr, true));
	for(auto sp : MatchHelpers::getOwnPlayers(*p)) {
		if(sp.get() == p) {
			continue;
		}
		double dist = MatchEntity::distanceBetween(*p, *sp);
		if(dist < 5.0)
			continue;
		if(dist > 35.0)
			continue;

		double thisscore = 1.0f;
		AbsVector3 tovec = MatchEntity::vectorFromTo(*p, *sp);
		if(!tovec.v.null()) {
			tovec.v.normalize();
			if(!MatchHelpers::attacksUp(*p))
				tovec.v.y = -tovec.v.y;
			thisscore = (tovec.v.y + 1.0) * 0.5;
		}
		else {
			continue;
		}

		assert(thisscore >= 0.0f);

		if(sp->isGoalkeeper())
			thisscore *= 0.2f;

		if(thisscore > mScore) {
			for(auto op : MatchHelpers::getOpposingPlayers(*sp)) {
				/* TODO: set maxOppDist and coeff to be dependent on team tactics */
				static const float maxOppDist = 4.0f;
				float dist = Common::Math::pointToLineDistance(p->getPosition().v,
						sp->getPosition().v,
						op->getPosition().v);
				float angToMe = p->getPosition().v.dot(op->getPosition().v);
				if(angToMe > 0.0f && dist < maxOppDist) {
					float decr = (maxOppDist - dist) / maxOppDist;
					float coeff = 0.5f;
					thisscore -= coeff * decr;
				}
			}
			thistgt = AIHelpers::getPassKickVector(*mPlayer, *sp);
			thisscore = AIHelpers::checkKickSuccess(*mPlayer, thistgt, thisscore);
			if(thisscore > mScore) {
				mScore = thisscore;
				tgtPlayer = sp.get();
				tgt = thistgt;
			}
		}
	}
	if(mScore >= -1.0f) {
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, tgtPlayer));
	}
}

const char* AIPassAction::mActionName = "Pass";

AILongPassAction::AILongPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = -1.0;
	AbsVector3 tgt;
	Player* tgtPlayer = nullptr;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p),
				nullptr, true));

	float myshotscore = mPlayer->getTeam()->getShotScoreAt(p->getPosition());
	float mypassscore = mPlayer->getTeam()->getPassScoreAt(p->getPosition());

	for(auto sp : MatchHelpers::getOwnPlayers(*p)) {
		if(sp.get() == p) {
			continue;
		}
		double dist = MatchEntity::distanceBetween(*p, *sp);
		if(dist < 25.0)
			continue;

		double thisscore = std::max(mPlayer->getTeam()->getShotScoreAt(sp->getPosition()) - myshotscore,
				0.5f * mPlayer->getTeam()->getPassScoreAt(sp->getPosition()) - mypassscore);

		tgt = AIHelpers::getPassKickVector(*mPlayer, *sp);
		thisscore = AIHelpers::checkKickSuccess(*mPlayer, tgt, thisscore);

		if(thisscore > 0.0f && thisscore > mScore) {
			mScore = thisscore;
			tgtPlayer = sp.get();
		}
	}
	if(mScore >= 0.0f) {
		tgt.v.z += tgt.v.length() * 0.4f;
		/* TODO: this coefficient should be dependent on air viscosity */
		tgt.v *= 0.4f;
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, tgtPlayer));
	}
}

const char* AILongPassAction::mActionName = "Long Pass";

AIFetchBallAction::AIFetchBallAction(const Player* p)
	: AIAction(mActionName, p)
{
	float maxdist = 20.0f;
	float dist = MatchEntity::distanceBetween(*p,
			*p->getMatch()->getBall());
	mScore = std::max(0.01f, (maxdist - dist) / maxdist);
	AbsVector3 tgtpos = p->getMatch()->getBall()->getPosition();
	mScore = AIHelpers::checkTacticArea(*p, mScore, tgtpos);
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos);
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
		if(op->getTeam()->isOffsidePosition(op->getPosition()))
			continue;

		float dangerousness = MatchHelpers::getOpposingTeam(*p)->getShotScoreAt(op->getPosition());
		bool alreadyguarded = false;
		for(auto pl : MatchHelpers::getOwnPlayers(*p)) {
			if(&*pl == p)
				continue;
			float disttoown = Common::Math::pointToLineDistance(op->getPosition().v,
					owngoal.v,
					pl->getPosition().v);
			if(disttoown < 1.0f) {
				alreadyguarded = true;
				break;
			}
		}
		AbsVector3 thispos = (op->getPosition().v + owngoal.v) * 0.5f;
		if(!alreadyguarded) {
			dangerousness = AIHelpers::checkTacticArea(*p, dangerousness, thispos);
			if(dangerousness > highestdangerousness) {
				highestdangerousness = dangerousness;
				tgtpos = thispos;
			}
		}
	}
	mScore = highestdangerousness;
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos, 1.0f);
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
	AbsVector3 blockpos = AbsVector3((op->getPosition().v + owngoal.v) * 0.5f);
	mScore = std::min(1.0f, (100.0f - disttogoal) / 50.0f);
	for(auto pl : MatchHelpers::getOwnPlayers(*p)) {
		if(&*pl == p || pl->isGoalkeeper())
			continue;
		float disttoown =
			Common::Math::pointToLineDistance(blockpos.v,
					owngoal.v,
					pl->getPosition().v);
		if(disttoown < 0.5f) {
			mScore = -1.0f;
			break;
		}
	}
	if(mScore > 0.0f)
		mScore = AIHelpers::checkTacticArea(*p, mScore, blockpos);
	mAction = AIHelpers::createMoveActionTo(*p, blockpos, 1.0f);
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
		if(pl->getTeam()->isOffsidePosition(op->getPosition()))
			continue;
		float thisscore = (50.0f - disttogoal) / 50.0f;
		if(thisscore > 0.0f) {
			thisscore = AIHelpers::checkTacticArea(*p, thisscore, pl->getPosition());
			if(thisscore > mScore) {
				mScore = thisscore;
				bestpos = pl->getPosition();
			}
		}
	}
	AbsVector3 tgtpos = AbsVector3((p->getPosition().v + bestpos.v) * 0.5f);
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos, 1.0f);
}

const char* AIBlockPassAction::mActionName = "Block pass";

AIGuardAreaAction::AIGuardAreaAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move to the correct X position
	float bestx = p->getMatch()->getPitchWidth() * 0.5f * p->getTactics().WidthPosition;
	mScore = 0.01f;
	AbsVector3 tgtpos = p->getPosition();
	tgtpos.v.x = bestx;
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos, 10.0f);
}

const char* AIGuardAreaAction::mActionName = "Guard area";

AITackleAction::AITackleAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to tackle the ball/opponent currently holding the ball
	float maxdist = 3.0f;
	Vector3 tacklevec = p->getMatch()->getBall()->getPosition().v +
		p->getMatch()->getBall()->getVelocity().v * 0.5f -
		p->getPosition().v;
	float dist = tacklevec.length();
	mScore = -1.0f;
	if(dist < maxdist) {
		Player* nearestopp = MatchHelpers::nearestOppositePlayerToBall(*p->getTeam());
		if(nearestopp->standing()) {
			float oppdist = MatchEntity::distanceBetween(*p,
					*nearestopp);
			const float maxOppDist = 2.0f;
			if(oppdist < maxOppDist) {
				float distToOwnGoal = (MatchHelpers::ownGoalPosition(*p).v -
						p->getMatch()->getBall()->getPosition().v).length();
				if(distToOwnGoal > 10.0f && distToOwnGoal < 80.0f) {
					mScore = 1.0f - (oppdist / maxOppDist);
				}
			}
		}
	}
	mAction = boost::shared_ptr<PlayerAction>(new TacklePA(tacklevec));
}

const char* AITackleAction::mActionName = "Tackle";


