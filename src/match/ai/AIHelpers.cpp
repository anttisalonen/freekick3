#include <stdlib.h>
#include <algorithm>

#include "match/ai/AIHelpers.h"
#include "match/Team.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"

std::shared_ptr<PlayerAction> AIHelpers::createMoveActionTo(const Player& p,
		const AbsVector3& pos)
{
	AbsVector3 v(pos);
	v.v -= p.getPosition().v;
	if(v.v.length() < 0.3f) {
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

AbsVector3 AIHelpers::getSupportingPosition(const Player& p)
{
	float best = 0.001f;
	AbsVector3 sp(p.getPosition());
	sp.v.y = 0.0f; // move stuck player towards middle
	const int range = 40;
	const int step = 5;
	int minx = int(p.getMatch()->getPitchWidth() * -0.5f + 1);
	int maxx = int(-minx);
	int miny = int(p.getMatch()->getPitchHeight() * -0.5f + 1);
	int maxy = int(-miny);
	for(int j = std::max(miny, (int)(p.getPosition().v.y - range));
			j <= std::min(maxy, (int)(p.getPosition().v.y + range));
			j += step) {
		for(int i = std::max(minx, (int)(p.getPosition().v.x - range));
				i <= std::min(maxx, (int)(p.getPosition().v.x + range));
				i += step) {
			AbsVector3 thispos(AbsVector3(i, j, 0));
			float thisvalue = p.getTeam()->getSupportingPositionScoreAt(thispos);
			if(thisvalue > best && MatchHelpers::nearestOwnPlayerTo(p, thispos)) {
				best = thisvalue;
				sp.v.x = i;
				sp.v.y = j;
				// printf("Best: (%3.3f, %3.3f) => %3.3f\n", sp.v.x, sp.v.y, best);
			}
		}
	}
	return sp;
}


