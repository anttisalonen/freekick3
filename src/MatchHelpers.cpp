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

bool MatchHelpers::allowedToKick(const Match& m,
		const Player& p)
{
	return !playing(m.getMatchHalf()) || playing(m.getPlayState()) ||
		m.getReferee()->isFirstTeamInControl() == p.getTeam()->isFirst();
}

