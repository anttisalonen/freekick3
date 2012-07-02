#include <assert.h>

#include "match/MatchHelpers.h"
#include "match/Team.h"

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
		myTeamInControl(p);
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
		if(!tp->standing())
			continue;
		float this_dist = (v.v - tp->getPosition().v).length();
		if(this_dist < smallest_dist) {
			smallest_dist = this_dist;
			np = &*tp;
		}
	}
	return np;
}

Player* MatchHelpers::nearestOppositePlayerToBall(const Team& t)
{
	const Team* t2 = t.isFirst() ? t.getMatch()->getTeam(1) : t.getMatch()->getTeam(0);
	return nearestOwnPlayerToBall(*t2);
}

bool MatchHelpers::nearestOwnPlayerTo(const Player& p, const AbsVector3& v)
{
	Player* np = nearestOwnPlayerTo(*p.getTeam(), v);
	return &p == np;
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

AbsVector3 MatchHelpers::ownGoalPosition(const Player& p)
{
	return ownGoalPosition(*p.getTeam());
}

AbsVector3 MatchHelpers::ownGoalPosition(const Team& t)
{
	const Match* m = t.getMatch();
	assert(m);
	if(attacksUp(t)) {
		return m->convertRelativeToAbsoluteVector(RelVector3(Vector3(0, -1, 0)));
	}
	else {
		return m->convertRelativeToAbsoluteVector(RelVector3(Vector3(0, 1, 0)));
	}
}

AbsVector3 MatchHelpers::oppositeGoalPosition(const Player& p)
{
	return oppositeGoalPosition(*p.getTeam());
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
	float zdiff = p.getMatch()->getBall()->getPosition().v.z - p.getPosition().v.z;
	return p.canKickBall() && allowedToKick(p) &&
		zdiff >= 0.0f && zdiff <= 1.0f &&
		MatchEntity::distanceBetween(p, *p.getMatch()->getBall()) <= MAX_KICK_DISTANCE;
}

bool MatchHelpers::myTeamInControl(const Player& p)
{
	return p.getMatch()->getReferee()->isFirstTeamInControl() == p.getTeam()->isFirst();
}

const std::vector<boost::shared_ptr<Player>>& MatchHelpers::getOpposingPlayers(const Team& t)
{
	return getTeamPlayers(*t.getMatch(), t.isFirst() ? 1 : 0);
}

const std::vector<boost::shared_ptr<Player>>& MatchHelpers::getOpposingPlayers(const Player& p)
{
	return getOpposingPlayers(*p.getTeam());
}

const std::vector<boost::shared_ptr<Player>>& MatchHelpers::getOwnPlayers(const Player& p)
{
	return getTeamPlayers(*p.getMatch(), p.getTeam()->isFirst() ? 0 : 1);
}

const std::vector<boost::shared_ptr<Player>>& MatchHelpers::getTeamPlayers(const Match& m, unsigned int idx)
{
	const Team* t = m.getTeam(idx);
	assert(t);
	return t->getPlayers();
}

bool MatchHelpers::attacksUp(const Player& p)
{
	return p.getTeam()->isFirst() == (p.getMatch()->getMatchHalf() <= MatchHalf::FirstHalf);
}

bool MatchHelpers::attacksUp(const Team& t)
{
	return t.isFirst() == (t.getMatch()->getMatchHalf() <= MatchHalf::FirstHalf);
}

const Team* MatchHelpers::getOpposingTeam(const Player& p)
{
	unsigned int idx = p.getTeam()->isFirst() ? 1 : 0;
	return p.getMatch()->getTeam(idx);
}

bool MatchHelpers::onPitch(const Match& m, const AbsVector3& v)
{
	float pw2 = m.getPitchWidth() / 2.0f;
	float ph2 = m.getPitchHeight() / 2.0f;
	return v.v.x >= -pw2 && v.v.y >= -ph2 && v.v.x <= pw2 && v.v.y <= ph2;
}

bool MatchHelpers::onPitch(const MatchEntity& m)
{
	return MatchHelpers::onPitch(*m.getMatch(), m.getPosition());
}

bool MatchHelpers::playersOnPause(const Match& m)
{
	for(int i = 0; i < 2; i++) {
		for(auto p : getTeamPlayers(m, i)) {
		if(MatchHelpers::onPitch(m, p->getPosition()))
			return false;
		}
	}
	return true;
}

bool MatchHelpers::playerPositionedForRestart(const Player& restarter, const Player& p)
{
	switch(p.getMatch()->getPlayState()) {
		case PlayState::OutKickoff:
			return &p == &restarter || onOwnSideAndReady(p);

		case PlayState::OutThrowin:
		case PlayState::OutCornerkick:
		case PlayState::OutIndirectFreekick:
		case PlayState::OutDirectFreekick:
		case PlayState::OutPenaltykick:
		case PlayState::OutDroppedball:
			/* TODO: move this magic constant */
			return !isOpposingPlayer(restarter, p) ||
				MatchEntity::distanceBetween(*p.getMatch()->getBall(), p) > 9.15f;

		case PlayState::OutGoalkick:
			return !isOpposingPlayer(restarter, p) ||
				!inOpposingPenaltyArea(p);

		case PlayState::InPlay:
			return true;
	}
	return true;
}

bool MatchHelpers::playersPositionedForRestart(const Match& m, const Player& restarter)
{
	switch(m.getPlayState()) {
		case PlayState::OutKickoff:
			for(int i = 0; i < 2; i++) {
				for(auto p : getTeamPlayers(m, i)) {
					if(!playerPositionedForRestart(restarter, *p)) {
						return false;
					}
				}
			}
			return true;

		case PlayState::OutThrowin:
		case PlayState::OutCornerkick:
		case PlayState::OutIndirectFreekick:
		case PlayState::OutDirectFreekick:
		case PlayState::OutPenaltykick:
		case PlayState::OutDroppedball:
			for(auto p : getOpposingPlayers(restarter)) {
				if(!playerPositionedForRestart(restarter, *p)) {
					return false;
				}
			}
			return true;

		case PlayState::OutGoalkick:
			for(auto p : getOpposingPlayers(restarter)) {
				if(!playerPositionedForRestart(restarter, *p)) {
					return false;
				}
			}
			return true;

		case PlayState::InPlay:
			return true;
	}
	return true;
}

// returns 0 if not in any penalty area,
// -1 if in lower, 1 if in upper penalty area.
int MatchHelpers::inPenaltyArea(const Player& p)
{
	const AbsVector3& v = p.getPosition();
	const Match& m = *p.getMatch();

	bool in_x = fabs(v.v.x) < 20.15f;
	float yp = v.v.y;

	if(!in_x)
		return 0;

	if(yp < m.getPitchHeight() * -0.5f + 16.5f)
		return -1;
	if(yp > m.getPitchHeight() * 0.5f - 16.5f)
		return 1;

	return 0;
}

bool MatchHelpers::inOwnPenaltyArea(const Player& p)
{
	int v = inPenaltyArea(p);
	if(v == 0)
		return false;
	return (v == -1 && MatchHelpers::attacksUp(p)) ||
			(v == 1 && !MatchHelpers::attacksUp(p));
}

bool MatchHelpers::inOpposingPenaltyArea(const Player& p)
{
	int v = inPenaltyArea(p);
	if(v == 0)
		return false;
	return (v == 1 && MatchHelpers::attacksUp(p)) ||
			(v == -1 && !MatchHelpers::attacksUp(p));
}

bool MatchHelpers::onOwnSide(const Player& p)
{
	if(fabs(p.getPosition().v.y > 1.0f) && attacksUp(p) != (p.getPosition().v.y < 0.0f)) {
		return false;
	}
	if(!onPitch(*p.getMatch(), p.getPosition())) {
		return false;
	}
	return true;
}

bool MatchHelpers::onOwnSideAndReady(const Player& p)
{
	if(!onOwnSide(p)) {
		return false;
	}
	if(p.getVelocity().v.length() > 1.0f) {
		return false;
	}
	return true;
}

bool MatchHelpers::isOpposingPlayer(const Player& p1, const Player& p2)
{
	return p1.getTeam()->isFirst() != p2.getTeam()->isFirst();
}

bool MatchHelpers::canGrabBall(const Player& p)
{
	const Ball* b = p.getMatch()->getBall();
	if(p.isGoalkeeper() && !b->grabbed() &&
			!myTeamInControl(p) &&
			inOwnPenaltyArea(p) &&
			onPitch(p)) {
		float distToBall = MatchEntity::distanceBetween(p, *b);
		float maxDist = p.standing() ? 1.5f : 1.0f;
		maxDist *= p.getSkills().GoalKeeping;
		float ballHeight = b->getPosition().v.z;
		float maxBallHeight = p.isAirborne() ? p.getPosition().v.z + 2.0f : p.standing() ? 2.0f : 0.5f;
		float minBallHeight = p.isAirborne() ? p.getPosition().v.z : 0.0f;
		if(maxDist >= distToBall && maxBallHeight >= ballHeight && minBallHeight <= ballHeight) {
			return true;
		}
	}

	return false;
}

bool MatchHelpers::goodKickingPosition(const Player& p, const AbsVector3& v)
{
	Vector3 pb = p.getMatch()->getBall()->getPosition().v - p.getPosition().v;
	Vector3 pt = (p.getMatch()->getBall()->getPosition().v + v.v) - p.getPosition().v;
	return pt.dot(pb) >= 0.0f;
}

float MatchHelpers::distanceToOwnGoal(const Player& p)
{
	return (p.getPosition().v - ownGoalPosition(p).v).length();
}

float MatchHelpers::distanceToOppositeGoal(const Player& p)
{
	return (p.getPosition().v - oppositeGoalPosition(p).v).length();
}


