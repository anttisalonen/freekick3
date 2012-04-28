#include "PlayerAIController.h"
#include "Match.h"
#include "Player.h"
#include "PlayerActions.h"

PlayerAIController::PlayerAIController(Player* p)
	: PlayerController(p)
{
}

std::shared_ptr<PlayerAction> PlayerAIController::act()
{
	switch(mPlayer->getMatch()->getMatchHalf()) {
		case MatchHalf::NotStarted:
			return createMoveActionTo(mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));
		case MatchHalf::HalfTimePause:
		case MatchHalf::Finished:
			return createMoveActionTo(mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getTeam()->getPausePosition()));
		default:
			break;
	}
	/* TODO */
	return std::shared_ptr<PlayerAction>(new IdlePA());
}

std::shared_ptr<PlayerAction> PlayerAIController::createMoveActionTo(const AbsVector3& pos) const
{
	AbsVector3 v(pos);
	v.v -= mPlayer->getPosition().v;
	if(v.v.length() < 1.0f) {
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		return std::shared_ptr<PlayerAction>(new RunToPA(v));
	}
}

