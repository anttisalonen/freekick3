#include "AIHelpers.h"

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


