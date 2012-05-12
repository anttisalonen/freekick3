#ifndef AIHELPERS_H
#define AIHELPERS_H

#include <memory>

#include "match/Player.h"
#include "match/PlayerActions.h"
#include "match/Distance.h"

class AIHelpers {
	public:
		static std::shared_ptr<PlayerAction> createMoveActionTo(const Player& p,
				const AbsVector3& pos);
		static std::shared_ptr<PlayerAction> createMoveActionToBall(const Player& p);
		static AbsVector3 getSupportingPosition(const Player& p);
		static AbsVector3 getPassKickVector(const Player& from, const AbsVector3& v);
		static AbsVector3 getPassKickVector(const Player& from, const Player& to);
};

#endif

