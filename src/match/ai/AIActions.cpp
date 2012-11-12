#include <assert.h>
#include <iostream>
#include <algorithm>

#include "common/Math.h"

#include "match/ai/AIActions.h"
#include "match/ai/AIHelpers.h"
#include "match/MatchHelpers.h"

using Common::Vector3;

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
	Vector3 shoottarget = MatchHelpers::oppositeGoalPosition(*p);
	Vector3 tgt = shoottarget;

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

	Vector3 vec(MatchHelpers::oppositePenaltySpotPosition(*p));
	vec -= p->getPosition();
	if(vec.length() > 32.0f) {
		return;
	}

	if((p->getPosition() - shoottarget).length() < 6.0f) {
		mScore = 1.0f;
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(shoottarget, nullptr, true));
		return;
	}

	const float riskcoeff = mPlayer->getTeam()->getAITacticParameters().ShootCloseCoefficient;
	/* controls how far the player might shoot from */
	float defaultScore = std::max(0.0f, 1.0f - (vec.length() - 16.0f) *
			(0.1f + 0.1f * (1.0f - riskcoeff)));
	float maxscore = -1.0f;
	const float maxOppDist = 2.0f;

	std::vector<Vector3> shoottargets;

	shoottargets.push_back(shoottarget);
	shoottargets.push_back(Vector3(shoottarget + Vector3(GOAL_WIDTH_2 * 0.75f, 0, 0)));
	shoottargets.push_back(Vector3(shoottarget + Vector3(-GOAL_WIDTH_2 * 0.75f, 0, 0)));

	for(auto thistgt : shoottargets) {
		float thisscore = defaultScore;
		for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
			float dist = Common::Math::pointToLineDistance(p->getPosition(),
					thistgt,
					op->getPosition());
			if(dist < maxOppDist) {
				thisscore -= riskcoeff * AIHelpers::scaledCoefficient(dist, maxOppDist);
				if(thisscore <= 0.0) {
					thisscore = 0.0f;
					break;
				}
			}
		}
		thisscore = AIHelpers::checkKickSuccess(*mPlayer, Vector3(thistgt - mPlayer->getPosition()),
							thisscore);
		if(thisscore > maxscore) {
			if((thistgt - p->getPosition()).length() > 15.0f)
				thistgt.z = (thistgt - p->getPosition()).length() *
					(0.04f + 0.08f * (1.0f - p->getSkills().ShotPower));
			maxscore = thisscore;
			tgt = thistgt;
		}
	}

	mScore = maxscore;
	mScore *= mPlayer->getTeam()->getAITacticParameters().ShootActionCoefficient;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, nullptr, true));
}

const char* AIShootAction::mActionName = "Shoot";

AIClearAction::AIClearAction(const Player* p)
	: AIAction(mActionName, p)
{
	Vector3 tgt = MatchHelpers::oppositeGoalPosition(*p);
	float distToOwnGoal = (MatchHelpers::ownGoalPosition(*p) - p->getMatch()->getBall()->getPosition()).length();
	float distToOpposingPlayer = MatchEntity::distanceBetween(*p->getMatch()->getBall(),
				*MatchHelpers::nearestOppositePlayerToBall(*p->getTeam()));

	const float maxDist = 20.0f;

	if(distToOpposingPlayer < 10.0f)
		mScore = AIHelpers::scaledCoefficient(distToOwnGoal, maxDist);
	else
		mScore = -1.0f;

	if(mScore > 0.0f) {
		Vector3 vecToBall = p->getMatch()->getBall()->getPosition() - p->getPosition();
		if(vecToBall.null()) {
			vecToBall = MatchHelpers::oppositeGoalPosition(*p) - p->getPosition();
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

		float dist1 = (MatchHelpers::ownGoalPosition(*p) - kickvec1).length();
		float dist2 = (MatchHelpers::ownGoalPosition(*p) - kickvec2).length();

		tgt = Vector3(dist1 > dist2 ? kickvec1 : kickvec2);
		tgt.z = tgt.length() * 0.8f;
		mScore = AIHelpers::checkKickSuccess(*mPlayer, tgt, mScore);
	}

	mScore *= mPlayer->getTeam()->getAITacticParameters().ClearActionCoefficient;

	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, nullptr, false));
}

const char* AIClearAction::mActionName = "Clear";

AIDribbleAction::AIDribbleAction(const Player* p)
	: AIAction(mActionName, p)
{
	float bestscore = -1.0f;
	Vector3 bestvec;
	std::vector<Vector3> tgtvectors;

	if(MatchHelpers::distanceToOwnGoal(*p) < 5.0f)
		return;

	/* TODO: this constant should depend on pitch */
	float dribblelen = 10.0f;
	const unsigned int numDirections = 16;
	for(int i = 0; i < numDirections; i++) {
		Vector3 vec;
		vec.x = dribblelen * sin(i * 2 * PI / float(numDirections));
		vec.y = dribblelen * cos(i * 2 * PI / float(numDirections));
		tgtvectors.push_back(vec);
	}

	for(auto vec : tgtvectors) {
		auto tgtpos = Vector3(p->getPosition() + vec.normalized() * 16.0f);
		if(!MatchHelpers::onPitch(*p->getMatch(), tgtpos))
			continue;

		float goalDistCoeff = AIHelpers::scaledCoefficient(MatchHelpers::distanceToOwnGoal(*p, tgtpos), 60.0f);
		float thisscore = 0.5f + goalDistCoeff * 0.5f;
		float depthCoeff = AIHelpers::getDepthCoefficient(*p, tgtpos);
		/* rather dribble towards opponent goal than away from it */
		for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
			float dist = Common::Math::pointToLineDistance(p->getPosition(),
					tgtpos,
					op->getPosition());
			static const float maxdist = dribblelen;
			if(dist < maxdist) {
				thisscore -= AIHelpers::scaledCoefficient(dist, maxdist) * (1.0f - depthCoeff);
				if(thisscore <= 0.0) {
					break;
				}
			}
		}

		thisscore = AIHelpers::checkKickSuccess(*mPlayer, vec, thisscore);

		if(thisscore > bestscore) {
			bestscore = thisscore;
			/* TODO: make scaling depend on player skill */
			bestvec = Vector3(vec.normalized() * 0.3f);
		}
	}
	mScore = bestscore;

	float dribblecoeff = mPlayer->getTeam()->getAITacticParameters().DribbleActionCoefficient;
	mScore *= dribblecoeff;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(bestvec));
}

const char* AIDribbleAction::mActionName = "Dribble";

AIPassAction::AIPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = -1.0;
	Vector3 tgt;
	Vector3 thistgt;
	Player* tgtPlayer = nullptr;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p),
				nullptr, true));

	const float riskcoeff = mPlayer->getTeam()->getAITacticParameters().PassRiskCoefficient;

	for(auto sp : MatchHelpers::getOwnPlayers(*p)) {
		if(sp.get() == p) {
			continue;
		}
		float dist = MatchEntity::distanceBetween(*p, *sp);
		if(dist < 10.0 && mPlayer->getMatch()->getPlayState() != PlayState::OutKickoff)
			continue;
		if(dist > 35.0)
			continue;

		float depthCoeff = AIHelpers::getDepthCoefficient(*p, p->getPosition());
		double thisscore = (AIHelpers::getPassForwardCoefficient(*p, *sp) +
			mPlayer->getTeam()->getAITacticParameters().PassActionCoefficient) * 0.5f;

		if(sp->isGoalkeeper())
			thisscore *= 0.2f;

		if(thisscore > mScore) {
			for(auto op : MatchHelpers::getOpposingPlayers(*sp)) {
				float oppdist = Common::Math::pointToLineDistance(p->getPosition(),
						sp->getPosition(),
						op->getPosition());
				float angToMe = p->getPosition().dot(op->getPosition());
				float maxoppdist = Common::clamp(4.0f, dist * 0.3f, 10.0f);

				if(p->isGoalkeeper())
					maxoppdist *= 2.0f;

				if(angToMe > 0.0f && oppdist < maxoppdist) {
					float decr = riskcoeff * (maxoppdist - oppdist) / maxoppdist;
					decr *= 1.0f + 8.0f * AIHelpers::scaledCoefficient(MatchHelpers::distanceToOwnGoal(*sp), 50.0f);
					thisscore -= decr * (1.0f - depthCoeff);
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
		mScore *= mPlayer->getTeam()->getAITacticParameters().PassActionCoefficient;
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, tgtPlayer));
	}
}

const char* AIPassAction::mActionName = "Pass";

AILongPassAction::AILongPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	mScore = -1.0;
	Vector3 tgt;
	Player* tgtPlayer = nullptr;
	mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(MatchHelpers::oppositeGoalPosition(*p),
				nullptr, true));

	const float myshotscore = mPlayer->getTeam()->getShotScoreAt(p->getPosition());
	const float mypassscore = mPlayer->getTeam()->getPassScoreAt(p->getPosition());

	for(auto sp : MatchHelpers::getOwnPlayers(*p)) {
		if(sp.get() == p) {
			continue;
		}
		double dist = MatchEntity::distanceBetween(*p, *sp);
		if(dist < 30.0)
			continue;

		if(MatchHelpers::distanceToOwnGoal(*sp) < 30.0f)
			continue;

		double thisscore = (AIHelpers::getPassForwardCoefficient(*p, *sp) +
				mPlayer->getTeam()->getAITacticParameters().LongPassActionCoefficient) * 0.5f;

		thisscore *= std::max(mPlayer->getTeam()->getShotScoreAt(sp->getPosition()) - myshotscore,
				0.5f * (mPlayer->getTeam()->getPassScoreAt(sp->getPosition()) - mypassscore));

		Vector3 thistgt = AIHelpers::getPassKickVector(*mPlayer, *sp);
		thisscore = AIHelpers::checkKickSuccess(*mPlayer, thistgt, thisscore);

		if(thisscore > mScore) {
			mScore = thisscore;
			tgt = thistgt;
			tgtPlayer = sp.get();
		}
	}

	if(mScore >= 0.0f) {
		mScore *= mPlayer->getTeam()->getAITacticParameters().LongPassActionCoefficient;
		/* TODO: these coefficients should be dependent on air viscosity */
		tgt.z += tgt.length() * 0.5f;
		tgt *= 0.5f;
		mAction = boost::shared_ptr<PlayerAction>(new KickBallPA(tgt, tgtPlayer));
	}
}

const char* AILongPassAction::mActionName = "Long Pass";

AIFetchBallAction::AIFetchBallAction(const Player* p)
	: AIAction(mActionName, p)
{
	float maxdist = 30.0f;
	float dist = MatchEntity::distanceBetween(*p,
			*p->getMatch()->getBall());
	Vector3 tgtpos = p->getMatch()->getBall()->getPosition();

	mScore = std::max(0.01f, AIHelpers::scaledCoefficient(dist, maxdist));
	mScore = AIHelpers::checkTacticArea(*p, mScore, tgtpos);
	mScore *= mPlayer->getTeam()->getAITacticParameters().FetchBallActionCoefficient;
	mAction = AIHelpers::createMoveActionToBall(*p);
}

const char* AIFetchBallAction::mActionName = "Fetch";

AIGuardAction::AIGuardAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move between opposing supporting player and own goal
	Vector3 owngoal = MatchHelpers::ownGoalPosition(*p);
	float highestdangerousness = -1.0f;
	Vector3 tgtpos(p->getPosition());
	for(auto op : MatchHelpers::getOpposingPlayers(*p)) {
		if(op->getTeam()->isOffsidePosition(op->getPosition()))
			continue;

		float dangerousness = MatchHelpers::getOpposingTeam(*p)->getShotScoreAt(op->getPosition());
		bool alreadyguarded = false;
		for(auto pl : MatchHelpers::getOwnPlayers(*p)) {
			if(&*pl == p)
				continue;
			float disttoown = Common::Math::pointToLineDistance(op->getPosition(),
					owngoal,
					pl->getPosition());
			if(disttoown < 1.0f) {
				alreadyguarded = true;
				break;
			}
		}
		Vector3 thispos = (op->getPosition() + owngoal) * 0.5f;
		if(!alreadyguarded) {
			dangerousness = AIHelpers::checkTacticArea(*p, dangerousness, thispos);
			if(dangerousness > highestdangerousness) {
				highestdangerousness = dangerousness;
				tgtpos = thispos;
			}
		}
	}
	mScore = highestdangerousness;
	/* drop guard score as it should be seen as backup action */
	mScore *= mPlayer->getTeam()->getAITacticParameters().GuardActionCoefficient * 0.5f;
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos, 1.0f);
}

const char* AIGuardAction::mActionName = "Guard";

AIBlockAction::AIBlockAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move between the opposing player holding the ball
	// and own goal
	Vector3 owngoal = MatchHelpers::ownGoalPosition(*p);
	const Player* op = MatchHelpers::nearestOppositePlayerToBall(*p->getTeam());
	float disttogoal = (owngoal - op->getPosition()).length();
	Vector3 blockpos = Vector3((op->getPosition() + owngoal) * 0.5f);
	mScore = std::min(1.0f, (100.0f - disttogoal) / 50.0f);
	for(auto pl : MatchHelpers::getOwnPlayers(*p)) {
		if(&*pl == p || pl->isGoalkeeper())
			continue;
		float disttoown =
			Common::Math::pointToLineDistance(blockpos,
					owngoal,
					pl->getPosition());
		if(disttoown < 0.5f) {
			mScore = -1.0f;
			break;
		}
	}
	if(mScore > 0.0f)
		mScore = AIHelpers::checkTacticArea(*p, mScore, blockpos);
	mScore *= mPlayer->getTeam()->getAITacticParameters().BlockActionCoefficient;
	mAction = AIHelpers::createMoveActionTo(*p, blockpos, 1.0f);
}

const char* AIBlockAction::mActionName = "Block";

AIBlockPassAction::AIBlockPassAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move between opposing supporting player and opponent holding the ball
	Vector3 owngoal = MatchHelpers::ownGoalPosition(*p);
	const Player* op = MatchHelpers::nearestOppositePlayerToBall(*p->getTeam());
	Vector3 bestpos(p->getPosition());
	mScore = -1.0f;
	for(auto pl : MatchHelpers::getOpposingPlayers(*p)) {
		if(op == &*pl)
			continue;
		float disttogoal = (owngoal - pl->getPosition()).length();
		if(disttogoal > 50.0f)
			continue;
		if(pl->getTeam()->isOffsidePosition(op->getPosition()))
			continue;
		float thisscore = (50.0f - disttogoal) / 50.0f;
		if(thisscore > 0.0f) {
			thisscore = AIHelpers::checkTacticArea(*p, thisscore, pl->getPosition());
			thisscore *= mPlayer->getTeam()->getAITacticParameters().BlockPassActionCoefficient;
			if(thisscore > mScore) {
				mScore = thisscore;
				bestpos = pl->getPosition();
			}
		}
	}
	Vector3 tgtpos = Vector3((op->getPosition() + bestpos) * 0.5f);
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos, 1.0f);
}

const char* AIBlockPassAction::mActionName = "Block pass";

AIGuardAreaAction::AIGuardAreaAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to move to the correct X position
	float bestx = p->getMatch()->getPitchWidth() * 0.5f * p->getTactics().WidthPosition;
	mScore = 0.05f;
	Vector3 tgtpos = p->getPosition();
	tgtpos.x = bestx;
	tgtpos.y = MatchHelpers::attacksUp(*p) ?
		std::min(p->getPosition().y, p->getMatch()->getBall()->getPosition().y) :
		std::max(p->getPosition().y, p->getMatch()->getBall()->getPosition().y);
	mScore *= mPlayer->getTeam()->getAITacticParameters().GuardAreaActionCoefficient;
	mAction = AIHelpers::createMoveActionTo(*p, tgtpos, 10.0f);
}

const char* AIGuardAreaAction::mActionName = "Guard area";

AITackleAction::AITackleAction(const Player* p)
	: AIAction(mActionName, p)
{
	// action to tackle the ball/opponent currently holding the ball
	float maxdist = 3.0f;
	Vector3 tacklevec = p->getMatch()->getBall()->getPosition() +
		p->getMatch()->getBall()->getVelocity() * 0.5f -
		p->getPosition();
	float dist = tacklevec.length();
	mScore = -1.0f;
	if(dist < maxdist) {
		Player* nearestopp = MatchHelpers::nearestOppositePlayerToBall(*p->getTeam());
		if(nearestopp->standing()) {
			float oppdist = MatchEntity::distanceBetween(*p,
					*nearestopp);
			const float maxOppDist = 2.0f;
			if(oppdist < maxOppDist) {
				float distToOwnGoal = (MatchHelpers::ownGoalPosition(*p) -
						p->getMatch()->getBall()->getPosition()).length();
				if(distToOwnGoal > 10.0f && distToOwnGoal < 80.0f) {
					mScore = 1.0f - (oppdist / maxOppDist);
					mScore *= mPlayer->getTeam()->getAITacticParameters().TackleActionCoefficient;
				}
			}
		}
	}
	mAction = boost::shared_ptr<PlayerAction>(new TacklePA(tacklevec));
}

const char* AITackleAction::mActionName = "Tackle";


