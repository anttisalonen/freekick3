#include <assert.h>

#include "MatchHelpers.h"

double MatchHelpers::distanceToPitch(const Match& m,
		const AbsVector3& v)
{
	float r = m.getPitchWidth() * 0.5f;
	float l = -r;
	float u = m.getPitchHeight() * 0.5f;
	float d = -u;
	if(v.v.x >= l && v.v.x <= r && v.v.y >= d && v.v.y <= u)
		return 0.0f;
	float dhoriz = v.v.x < l ? l - v.v.x : v.v.x - r;
	float dvert  = v.v.y < d ? d - v.v.y : v.v.y - u;
	if(dvert < 0)
		return dhoriz;
	else if(dhoriz < 0)
		return dvert;
	else
		return sqrt(dvert * dvert + dhoriz * dhoriz);
}

bool MatchHelpers::allowedToKick(const Player& p)
{
	const Match* m = p.getMatch();
	assert(m);
	return !playing(m->getMatchHalf()) || playing(m->getPlayState()) ||
		m->getReferee()->isFirstTeamInControl() == p.getTeam()->isFirst();
}

Player* MatchHelpers::nearestOwnPlayerToPlayer(const Team& t, const Player& p)
{
	return nearestOwnPlayerTo(t, p.getPosition());
}

Player* MatchHelpers::nearestOwnPlayerToBall(const Team& t)
{
	return nearestOwnPlayerTo(t, t.getMatch()->getBall()->getPosition());
}

Player* MatchHelpers::nearestOwnPlayerTo(const Team& t, const AbsVector3& v)
{
	Player* np = nullptr;
	float smallest_dist = 1000000.0f;
	for(const auto& tp : t.getPlayers()) {
		float this_dist = (v.v - tp->getPosition().v).length();
		if(this_dist < smallest_dist) {
			smallest_dist = this_dist;
			np = &*tp;
		}
	}
	return np;
}

bool MatchHelpers::nearestOwnPlayerTo(const Player& p, const AbsVector3& v)
{
	Player* np = nearestOwnPlayerTo(*p.getTeam(), v);
	return &p == np;
}

AbsVector3 MatchHelpers::oppositeGoalPosition(const Player& p)
{
	const Match* m = p.getMatch();
	assert(m);
	if(p.getTeam()->isFirst() == (m->getMatchHalf() == MatchHalf::FirstHalf)) {
		return m->convertRelativeToAbsoluteVector(RelVector3(Vector3(0, 1, 0)));
	}
	else {
		return m->convertRelativeToAbsoluteVector(RelVector3(Vector3(0, -1, 0)));
	}
}

bool MatchHelpers::canKickBall(const Player& p)
{
	return p.canKickBall() && allowedToKick(p) &&
		MatchEntity::distanceBetween(p, *p.getMatch()->getBall()) <= MAX_KICK_DISTANCE;
}

