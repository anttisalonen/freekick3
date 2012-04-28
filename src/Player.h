#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

class Match;
class PlayerAction;
class PlayerController;

class Player {
	public:
		Player(Match* match);
		std::shared_ptr<PlayerAction> act();
	private:
		Match* mMatch;
		std::shared_ptr<PlayerController> mController;
		friend class PlayerAIController;
};

#endif


