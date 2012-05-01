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
	return oppositeGoalPosition(*p.getTeam());
}

AbsVector3 MatchHelpers::oppositePenaltySpotPosition(const Player& p)
{
	AbsVector3 v(oppositeGoalPosition(p));
	if(v.v.y > 0.0f)
		v.v.y -= 11.0f;
	else
		v.v.y += 11.0f;
	return v;
}

AbsVector3 MatchHelpers::oppositeGoalPosition(const Team& t)
{
	const Match* m = t.getMatch();
	assert(m);
	if(attacksUp(t)) {
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

bool MatchHelpers::myTeamInControl(const Player& p)
{
	return p.getMatch()->getReferee()->isFirstTeamInControl() == p.getTeam()->isFirst();
}

const std::vector<std::shared_ptr<Player>>& MatchHelpers::getOpposingPlayers(const Team& t)
{
	return getTeamPlayers(*t.getMatch(), t.isFirst() ? 1 : 0);
}

const std::vector<std::shared_ptr<Player>>& MatchHelpers::getOpposingPlayers(const Player& p)
{
	return getOpposingPlayers(*p.getTeam());
}

const std::vector<std::shared_ptr<Player>>& MatchHelpers::getOwnPlayers(const Player& p)
{
	return getTeamPlayers(*p.getMatch(), p.getTeam()->isFirst() ? 0 : 1);
}

const std::vector<std::shared_ptr<Player>>& MatchHelpers::getTeamPlayers(const Match& m, unsigned int idx)
{
	const Team* t = m.getTeam(idx);
	assert(t);
	return t->getPlayers();
}

bool MatchHelpers::attacksUp(const Player& p)
{
	return p.getTeam()->isFirst() == (p.getMatch()->getMatchHalf() == MatchHalf::FirstHalf);
}

bool MatchHelpers::attacksUp(const Team& t)
{
	return t.isFirst() == (t.getMatch()->getMatchHalf() == MatchHalf::FirstHalf);
}


