#include <algorithm>

#include "AIHelpers.h"
#include "Team.h"
#include "Match.h"
#include "MatchHelpers.h"

std::shared_ptr<PlayerAction> AIHelpers::createMoveActionTo(const Player& p,
		const AbsVector3& pos)
{
	AbsVector3 v(pos);
	v.v -= p.getPosition().v;
	if(v.v.length() < 0.3f) {
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		return std::shared_ptr<PlayerAction>(new RunToPA(v));
	}
}

AbsVector3 AIHelpers::getSupportingPosition(const Player& p)
{
	float best = -1.0f;
	AbsVector3 sp(p.getPosition());
	const int range = 32;
	const int step = 8;
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


