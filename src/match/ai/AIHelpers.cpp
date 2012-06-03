#include <stdlib.h>
#include <algorithm>

#include "common/Math.h"

#include "match/Team.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"

#include "match/ai/AIHelpers.h"

std::shared_ptr<PlayerAction> AIHelpers::createMoveActionTo(const Player& p,
		const AbsVector3& pos, float threshold)
{
	AbsVector3 v(pos);
	v.v -= p.getPosition().v;
	if(v.v.length() < threshold) {
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		const AbsVector3& vel = p.getVelocity();
		if(vel.v.length() > 1.0f) {
			double dotp = v.v.normalized().dot(vel.v.normalized());
			if(fabs(dotp) < 0.5f) {
				// bring to halt first
				return std::shared_ptr<PlayerAction>(new IdlePA());
			}
		}
		return std::shared_ptr<PlayerAction>(new RunToPA(v));
	}
}

std::shared_ptr<PlayerAction> AIHelpers::createMoveActionToBall(const Player& p)
{
	const Ball* b = p.getMatch()->getBall();
	const AbsVector3& vel = b->getVelocity();
	if(vel.v.length() < 10.0f) {
		return createMoveActionTo(p, b->getPosition());
	}
	else {
		AbsVector3 tgt(b->getPosition().v + vel.v * 1.0f);
		if(MatchHelpers::onPitch(*p.getMatch(), tgt))
			return createMoveActionTo(p, tgt);
		else
			return createMoveActionTo(p, b->getPosition());
	}
}

AbsVector3 AIHelpers::getShotPosition(const Player& p)
{
	AbsVector3 v = getPositionByFunc(p, [&](const AbsVector3& v) { return p.getTeam()->getShotScoreAt(v); });
	if((v.v - p.getPosition().v).length() > 2.0f)
		return v;
	else
		return p.getPosition();
}

AbsVector3 AIHelpers::getPassPosition(const Player& p)
{
	AbsVector3 v = getPositionByFunc(p, [&](const AbsVector3& v) { return p.getTeam()->getPassScoreAt(v); });
	if((v.v - p.getPosition().v).length() > 2.0f)
		return v;
	else
		return p.getPosition();
}

AbsVector3 AIHelpers::getPositionByFunc(const Player& p, std::function<float (const AbsVector3& v)> func)
{
	float best = 0.001f;
	AbsVector3 sp(p.getPosition());
	const int range = 60;
	const int step = 5;
	int minx = int(p.getMatch()->getPitchWidth()  * -0.5f + 1);
	int maxx = int(p.getMatch()->getPitchWidth()  *  0.5f - 1);
	int miny = int(p.getMatch()->getPitchHeight() * -0.5f + 1);
	int maxy = int(p.getMatch()->getPitchHeight() *  0.5f - 1);

	for(int j = std::max(miny, (int)(p.getPosition().v.y - range));
			j <= std::min(maxy, (int)(p.getPosition().v.y + range));
			j += step) {
		for(int i = std::max(minx, (int)(p.getPosition().v.x - range));
				i <= std::min(maxx, (int)(p.getPosition().v.x + range));
				i += step) {
			AbsVector3 thispos(AbsVector3(i, j, 0));
			if(MatchHelpers::nearestOwnPlayerTo(p, thispos)) {
				float thisvalue = func(thispos);
				if(thisvalue > best) {
					thisvalue = AIHelpers::checkTacticArea(p, thisvalue, thispos);
					if(thisvalue > best) {
						best = thisvalue;
						sp.v.x = i;
						sp.v.y = j;
					}
				}
			}
		}
	}

	return sp;
}

AbsVector3 AIHelpers::getPassKickVector(const Player& from, const Player& to)
{
	return getPassKickVector(from, to.getPosition(), to.getVelocity());
}

AbsVector3 AIHelpers::getPassKickVector(const Player& from, const AbsVector3& pos, const AbsVector3& vel)
{
	AbsVector3 tgt = AbsVector3(pos.v + vel.v * 1.0f - from.getPosition().v);
	float powercoeff = std::max(0.3, 1.6 * tgt.v.length() / from.getMaximumShotPower());
	tgt.v.normalize();
	tgt.v *= powercoeff;
	return tgt;
}

AbsVector3 AIHelpers::getPassKickVector(const Player& from, const AbsVector3& to)
{
	return getPassKickVector(from, to, AbsVector3());
}

float AIHelpers::checkTacticArea(const Player& p, float score, const AbsVector3& pos)
{
	float bestx = p.getMatch()->getPitchWidth() * 0.5f * p.getTactics().WidthPosition;
	float maxDist = p.getMatch()->getPitchWidth() * 0.5f * p.getTactics().Radius;
	assert(maxDist != 0.0f);
	float dist = fabs(pos.v.x - bestx);
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


