#include <stdlib.h>
#include <algorithm>

#include "common/Math.h"
#include "common/Steering.h"

#include "match/Team.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"

#include "match/ai/AIHelpers.h"

using Common::Vector3;

boost::shared_ptr<PlayerAction> AIHelpers::createMoveActionTo(const Player& p,
		const Vector3& pos, float threshold)
{
	Vector3 v(pos);
	v -= p.getPosition();
	if(v.length() < threshold) {
		return boost::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		const Vector3& vel = p.getVelocity();
		if(vel.length() > 1.0f) {
			double dotp = v.normalized().dot(vel.normalized());
			if(fabs(dotp) < 0.5f) {
				// bring to halt first
				return boost::shared_ptr<PlayerAction>(new IdlePA());
			}
		}
		if(pos.z > 1.7f && pos.z < 2.5f && !p.isAirborne() && v.length() < 1.5f) {
			v.normalize();
			v.z += 1.0f;
			return boost::shared_ptr<PlayerAction>(new JumpToPA(v));
		}
		else {
			return boost::shared_ptr<PlayerAction>(new RunToPA(v));
		}
	}
}

boost::shared_ptr<PlayerAction> AIHelpers::createMoveActionToBall(const Player& p)
{
	Common::Steering s(p);
	const Ball* b = p.getMatch()->getBall();
	float ret1, ret2;
	auto pos = b->getPosition() - p.getPosition();
	auto vel = b->getVelocity();
	auto c = p.getRunSpeed();
	bool ret = Common::Math::tps(pos, vel, c, ret1, ret2);
	if(!ret || (ret1 <= 0.0f && ret2 <= 0.0f)) {
		auto vec = s.pursuit(*b);
		return createMoveActionTo(p, vec);
	} else {
		auto t = ret1 > 0.0f && ret1 < ret2 ? ret1 : ret2;
		auto intersectionPoint = pos + vel * t;
		return createMoveActionTo(p, p.getPosition() + intersectionPoint);
	}
}

Vector3 AIHelpers::getShotPosition(const Player& p)
{
	Vector3 v = getPositionByFunc(p, [&](const Vector3& vp) { return p.getTeam()->getShotScoreAt(vp); });
	/* NOTE: this constant basically defines how far in offside a forward will stand. */
	if((v - p.getPosition()).length() > 0.5f)
		return v;
	else
		return p.getPosition();
}

Vector3 AIHelpers::getPassPosition(const Player& p)
{
	Vector3 v = getPositionByFunc(p, [&](const Vector3& vp) { return p.getTeam()->getPassScoreAt(vp); });
	if((v - p.getPosition()).length() > 2.0f)
		return v;
	else
		return p.getPosition();
}

Vector3 AIHelpers::getPositionByFunc(const Player& p, std::function<float (const Vector3& v)> func)
{
	float best = 0.001f;
	Vector3 sp(p.getPosition());
	const int range = 100;
	const int step = 3;
	int minx = int(p.getMatch()->getPitchWidth()  * -0.5f + 1);
	int maxx = int(p.getMatch()->getPitchWidth()  *  0.5f - 1);
	int miny = int(p.getMatch()->getPitchHeight() * -0.5f + 1);
	int maxy = int(p.getMatch()->getPitchHeight() *  0.5f - 1);

	for(int j = std::max(miny, (int)(p.getPosition().y - range));
			j <= std::min(maxy, (int)(p.getPosition().y + range));
			j += step) {
		for(int i = std::max(minx, (int)(p.getPosition().x - range));
				i <= std::min(maxx, (int)(p.getPosition().x + range));
				i += step) {
			Vector3 thispos(Vector3(i, j, 0));
			float thisvalue = func(thispos);
			if(thisvalue > best) {
				thisvalue = AIHelpers::checkTacticArea(p, thisvalue, thispos);
				if(thisvalue > best) {
					best = thisvalue;
					sp.x = i;
					sp.y = j;
				}
			}
		}
	}

	return sp;
}

Vector3 AIHelpers::getPassKickVector(const Player& from, const Player& to)
{
	return getPassKickVector(from, to.getPosition(), to.getVelocity());
}

Vector3 AIHelpers::getPassKickVector(const Player& from, const Vector3& pos, const Vector3& vel)
{
	Vector3 tgt = Vector3(pos + vel * 0.5f - from.getPosition());
	float powercoeff = std::max(0.6, 1.3 * tgt.length() / from.getMaximumShotPower());
	tgt.normalize();
	tgt *= powercoeff;
	return tgt;
}

Vector3 AIHelpers::getPassKickVector(const Player& from, const Vector3& to)
{
	return getPassKickVector(from, to, Vector3());
}

float AIHelpers::checkTacticArea(const Player& p, float score, const Vector3& pos)
{
	float bestx = p.getMatch()->getPitchWidth() * 0.5f * p.getTactics().WidthPosition;
	float maxDist = p.getMatch()->getPitchWidth() * 0.5f * p.getTactics().Radius;
	assert(maxDist != 0.0f);
	float dist = fabs(pos.x - bestx);
	float val;
	val = Common::clamp(0.0f, 1.0f - (dist / maxDist), 1.0f);
	// printf("dist: %3.1f - maxdist: %3.1f - should: %3.1f - is: %3.1f - val: %3.3f\n", dist, maxDist, bestx, myx, val);
	return score * val;
}

float AIHelpers::scaledDistanceFrom(float dist, float opt)
{
	float distFromOptimum = fabs(opt - dist);
	return std::max(0.0f, (opt - distFromOptimum) / opt);
}

float AIHelpers::scaledCoefficient(float dist, float maximum)
{
	assert(maximum != 0.0f);
	assert(dist >= 0.0f);
	return std::max(0.0f, (maximum - dist) / maximum);
}

float AIHelpers::checkKickSuccess(const Player& p, const Vector3& v, float score)
{
	if(score < 0.0f) {
		return score;
	}
	if(!MatchHelpers::goodKickingPosition(p, v)) {
		if(!playing(p.getMatch()->getPlayState())) {
			return -1.0f;
		}
		else {
			const Player* opp = MatchHelpers::nearestOppositePlayerToBall(*p.getTeam());
			float dist = MatchEntity::distanceBetween(*opp, p);
			float coeff = scaledCoefficient(dist, 5.0f);
			return score * coeff;
		}
	}
	else {
		return score;
	}
}

float AIHelpers::getPassForwardCoefficient(const Player& p, const Player& tp)
{
	Vector3 tovec = MatchEntity::vectorFromTo(p, tp);
	if(!tovec.null()) {
		tovec.normalize();
		if(!MatchHelpers::attacksUp(p))
			tovec.y = -tovec.y;
		float val = (tovec.y + 1.0) * 0.5;
		assert(val >= 0.0f);
		return val;
	}
	else {
		return 0.0f;
	}
}

float AIHelpers::getDepthCoefficient(const Team& p, const Vector3& v)
{
	float depthCoeff = 0.5f + 0.5f * (v.y / (0.5f * p.getMatch()->getPitchHeight()));
	if(!MatchHelpers::attacksUp(p))
		depthCoeff = 1.0f - depthCoeff;

	return depthCoeff;
}

float AIHelpers::getDepthCoefficient(const Player& p, const Vector3& v)
{
	return getDepthCoefficient(*p.getTeam(), v);
}

bool AIHelpers::opponentAttacking(const Player& p)
{
	if(!MatchHelpers::myTeamInControl(p))
		return true;

	auto b = p.getTeam()->getMatch()->getBall();
	auto grabber = b->getGrabber();
	if(grabber && grabber->getTeam() == p.getTeam())
		return false;

	float ourDist = MatchEntity::distanceBetween(*MatchHelpers::nearestOwnPlayerToBall(*p.getTeam()), *b);
	float theirDist = MatchEntity::distanceBetween(*MatchHelpers::nearestOppositePlayerToBall(*p.getTeam()), *b);
	if(theirDist < ourDist)
		return true;

	return false;
}


