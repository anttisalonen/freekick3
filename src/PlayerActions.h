#ifndef PLAYERACTIONS_H
#define PLAYERACTIONS_H

class Match;
class Player;

class PlayerAction {
	public:
		virtual ~PlayerAction() { }
		virtual void applyPlayerAction(Match& match, const Player& p, double time) = 0;
};

class IdlePA : public PlayerAction {
	public:
		void applyPlayerAction(Match& match, const Player& p, double time);
};

#endif

