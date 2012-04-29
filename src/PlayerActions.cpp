#include "PlayerActions.h"
#include "Match.h"
#include "Player.h"

void IdlePA::applyPlayerAction(Match& match, Player& p, double time)
{
	p.setVelocity(AbsVector3());
	return;
}

RunToPA::RunToPA(const AbsVector3& v)
	: mDiff(v)
{
}

void RunToPA::applyPlayerAction(Match& match, Player& p, double time)
{
	if(mDiff.v.length() < 0.1f)
		return;
	AbsVector3 v(mDiff.v.normalized());
	v.v *= p.getRunSpeed();
	p.setVelocity(v);
}

