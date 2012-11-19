#include <assert.h>

#include "match/MatchHelpers.h"
#include "match/Team.h"

using Common::Vector3;

double MatchHelpers::distanceToPitch(const Match& m,
		const Vector3& v)
{
	float r = m.getPitchWidth() * 0.5f;
	float l = -r;
	float u = m.getPitchHeight() * 0.5f;
	float d = -u;
	if(v.x >= l && v.x <= r && v.y >= d && v.y <= u)
		return 0.0f;
	float dhoriz = v.x < l ? l - v.x : v.x - r;
	float dvert  = v.y < d ? d - v.y : v.y - u;
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
		(myTeamInControl(p) && m->getReferee()->canKickBall(p));
}

Player* MatchHelpers::nearestOwnPlayerToPlayer(const Team& t, const Player& p)
{
	return nearestOwnPlayerTo(t, p.getPosition());
}

Player* MatchHelpers::nearestOwnFieldPlayerToBall(const Team& t)
{
	return nearestOwnPlayerTo(t, t.getMatch()->getBall()->getPosition(), false);
}

Player* MatchHelpers::nearestOwnPlayerToBall(const Team& t)
{
	return nearestOwnPlayerTo(t, t.getMatch()->getBall()->getPosition());
}

Player* MatchHelpers::nearestOwnPlayerTo(const Team& t, const Vector3& v, bool goalkeepers)
{
	Player* np = nullptr;
	float smallest_dist = 1000000.0f;
	for(const auto& tp : t.getPlayers()) {
		if(!tp->standing())
			continue;
		if(tp->isGoalkeeper() && !goalkeepers)
			continue;
		float this_dist = (v - tp->getPosition()).length();
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

bool MatchHelpers::nearestOwnPlayerTo(const Player& p, const Vector3& v)
{
	Player* np = nearestOwnPlayerTo(*p.getTeam(), v);
	return &p == np;
}

Vector3 MatchHelpers::oppositePenaltySpotPosition(const Player& p)
{
	Vector3 v(oppositeGoalPosition(p));
	if(v.y > 0.0f)
		v.y -= 11.0f;
	else
		v.y += 11.0f;
	return v;
}

Vector3 MatchHelpers::ownGoalPosition(const Player& p)
{
	return ownGoalPosition(*p.getTeam());
}

Vector3 MatchHelpers::ownGoalPosition(const Team& t)
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

Vector3 MatchHelpers::oppositeGoalPosition(const Player& p)
{
	return oppositeGoalPosition(*p.getTeam());
}

Vector3 MatchHelpers::oppositeGoalPosition(const Team& t)
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

bool MatchHelpers::ballInHeadingHeight(const Player& p)
{
	float zdiff = p.getMatch()->getBall()->getPosition().z - p.getPosition().z;
	return zdiff >= 1.7f && zdiff <= 1.9f;
}

bool MatchHelpers::canKickBall(const Player& p)
{
	Vector3 v1 = p.getPosition();
	Vector3 v2 = p.getMatch()->getBall()->getPosition();
	float zdiff = v2.z - v1.z;
	v1.z = v2.z = 0.0f;
	float planediff = (v2 - v1).length();

	return p.canKickBall() && allowedToKick(p) &&
		((zdiff >= -0.3f && zdiff <= 0.7f) || ballInHeadingHeight(p)) &&
		planediff <= MAX_KICK_DISTANCE;
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
	return attacksUp(*p.getTeam());
}

bool MatchHelpers::attacksUp(const Team& t)
{
	auto mh = t.getMatch()->getMatchHalf();
	if(mh == MatchHalf::PenaltyShootout) {
		return t.isFirst() == t.getMatch()->getPenaltyShootout().firstTeamKicksNext();
	}

	return t.isFirst() == (mh <= MatchHalf::FirstHalf ||
		(mh <= MatchHalf::ExtraTimeFirstHalf &&
		mh >= MatchHalf::FullTimePauseEnd));
}

const Team* MatchHelpers::getOpposingTeam(const Player& p)
{
	unsigned int idx = p.getTeam()->isFirst() ? 1 : 0;
	return p.getMatch()->getTeam(idx);
}

bool MatchHelpers::onPitch(const Match& m, const Vector3& v)
{
	float pw2 = m.getPitchWidth() / 2.0f;
	float ph2 = m.getPitchHeight() / 2.0f;
	return v.x >= -pw2 && v.y >= -ph2 && v.x <= pw2 && v.y <= ph2;
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

/* NOTE: this must be kept somewhat aligned with
 * AIPlayStates::actOnRestart to prevent blocking */
bool MatchHelpers::playerPositionedForRestart(const Player& restarter, const Player& p)
{
	switch(p.getMatch()->getPlayState()) {
		case PlayState::OutKickoff:
			return &p == &restarter || onOwnSideAndReady(p);

		case PlayState::OutThrowin:
		case PlayState::OutCornerkick:
		case PlayState::OutIndirectFreekick:
		case PlayState::OutDirectFreekick:
		case PlayState::OutDroppedball:
			/* TODO: move this magic constant */
			return !isOpposingPlayer(restarter, p) ||
				MatchEntity::distanceBetween(*p.getMatch()->getBall(), p) > 9.15f;

		case PlayState::OutPenaltykick:
			{
				bool nearball = MatchEntity::distanceBetween(*p.getMatch()->getBall(), p) < 9.15f;
				bool inpenaltyarea = (isOpposingPlayer(restarter, p) && inOwnPenaltyArea(p)) ||
					(!isOpposingPlayer(restarter, p) && inOpposingPenaltyArea(p));
				bool isrestarter = &p == &restarter;
				bool isgoalie = isOpposingPlayer(restarter, p) && p.isGoalkeeper();
				bool nearowngoalline = fabs(p.getPosition().y - MatchHelpers::ownGoalPosition(p).y) < 0.5f;

				if(isrestarter) {
					return nearball;
				}
				else if(isgoalie) {
					return nearowngoalline; 
				}
				else {
					return !nearball && !inpenaltyarea;
				}
			}

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
	return inPenaltyArea(*p.getMatch(), p.getPosition());
}

int MatchHelpers::inPenaltyArea(const Match& m, const Vector3& v)
{
	bool in_x = fabs(v.x) < 20.15f;
	float yp = v.y;

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
	if(fabs(p.getPosition().y > 1.0f) && attacksUp(p) != (p.getPosition().y < 0.0f)) {
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
	if(p.getVelocity().length() > 1.0f) {
		return false;
	}
	return true;
}

bool MatchHelpers::isOpposingPlayer(const Player& p1, const Player& p2)
{
	return p1.getTeam()->isFirst() != p2.getTeam()->isFirst();
}

bool MatchHelpers::grabBallAllowed(const Player& p)
{
	if(p.isGoalkeeper() && !p.getMatch()->getBall()->grabbed() &&
			!myTeamInControl(p) &&
			inOwnPenaltyArea(p) &&
			onPitch(p) && playing(p.getMatch()->getPlayState())) {
		return true;
	} else {
		return false;
	}
}

bool MatchHelpers::canGrabBall(const Player& p)
{
	const Ball* b = p.getMatch()->getBall();
	if(grabBallAllowed(p)) {
		if(canKickBall(p))
			return true;

		float distToBall = MatchEntity::distanceBetween(p, *b);
		float maxDist = p.standing() ? 0.5f : 0.0f;
		float gk = p.getSkills().GoalKeeping;
		maxDist += sqrt(gk);
		float ballHeight = b->getPosition().z;
		float maxBallHeight = p.isAirborne() ? p.getPosition().z + 2.0f : p.standing() ? 2.0f : 0.5f;
		float minBallHeight = p.isAirborne() ? p.getPosition().z - (0.7f * gk) : 0.0f;
		if(maxDist >= distToBall && maxBallHeight >= ballHeight && minBallHeight <= ballHeight) {
			return true;
		}
	}

	return false;
}

bool MatchHelpers::goodKickingPosition(const Player& p, const Vector3& v)
{
	return true;
}

float MatchHelpers::distanceToOwnGoal(const Player& p)
{
	return (p.getPosition() - ownGoalPosition(p)).length();
}

float MatchHelpers::distanceToOppositeGoal(const Player& p)
{
	return (p.getPosition() - oppositeGoalPosition(p)).length();
}

float MatchHelpers::distanceToOwnGoal(const Player& p, const Common::Vector3& v)
{
	return (v - ownGoalPosition(p)).length();
}

float MatchHelpers::distanceToOppositeGoal(const Player& p, const Common::Vector3& v)
{
	return (v - oppositeGoalPosition(p)).length();
}

bool MatchHelpers::playerBlockingRestart(const Player& p)
{
	const Player* restarter;
	if(myTeamInControl(p)) {
		restarter = nearestOwnPlayerToBall(*p.getTeam());
	}
	else {
		restarter = nearestOppositePlayerToBall(*p.getTeam());
	}

	return &p != restarter && (!playerPositionedForRestart(*restarter, p) ||
		(p.getMatch()->getBall()->grabbed() &&
		 !myTeamInControl(p) &&
		 inOpposingPenaltyArea(p)));
}

Common::Vector3 MatchHelpers::playerJumpVelocity(const Player& p, const Common::Vector3& dir)
{
	if(!p.standing() || p.isAirborne() || dir.z < 0.01f) {
		return Vector3();
	}
	if(dir.length() < 0.1f) {
		return Vector3();
	}
	Vector3 v(dir.normalized());
	if(p.isGoalkeeper())
		v *= 1.0f + 2.0f * p.getSkills().GoalKeeping;
	else
		v *= 1.5f + 1.0f * p.getSkills().Heading;
	v *= 3.0f;
	v.z = Common::clamp(1.5f, v.length(), 4.5f);
	return v;
}

