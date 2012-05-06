#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <memory>

class Player;
class PlayerAction;
enum class MatchHalf;

class PlayerController {
	public:
		inline PlayerController(Player* p);
		virtual ~PlayerController() { }
		virtual std::shared_ptr<PlayerAction> act(double time) = 0;
		virtual void matchHalfChanged(MatchHalf m) { }
		inline void setPlayer(Player* p);
	protected:
		Player* mPlayer;
};

PlayerController::PlayerController(Player* p)
	: mPlayer(p)
{
}

void PlayerController::setPlayer(Player* p)
{
	mPlayer = p;
}

#endif

