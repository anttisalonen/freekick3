#ifndef AIHELPERS_H
#define AIHELPERS_H

#include <memory>

#include "Player.h"
#include "PlayerActions.h"
#include "Distance.h"

class AIHelpers {
	public:
		static std::shared_ptr<PlayerAction> createMoveActionTo(const Player& p,
				const AbsVector3& pos);
};

#endif

