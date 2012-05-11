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
};

#endif

