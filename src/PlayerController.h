#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <memory>

class Player;
class PlayerAction;

class PlayerController {
	public:
		inline PlayerController(Player* p);
		virtual ~PlayerController() { }
		virtual std::shared_ptr<PlayerAction> act(double time) = 0;
	protected:
		Player* mPlayer;
};

PlayerController::PlayerController(Player* p)
	: mPlayer(p)
{
}

#endif

