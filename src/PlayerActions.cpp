#include "PlayerActions.h"
#include "Match.h"
#include "Player.h"

void IdlePA::applyPlayerAction(Match& match, Player& p, double time)
{
	return;
}

RunToPA::RunToPA(const AbsVector3& v)
	: mDiff(v)
{
}

void RunToPA::applyPlayerAction(Match& match, Player& p, double time)
{
	AbsVector3 v(mDiff.v);
	v.v *= time;
	p.move(v);
}


