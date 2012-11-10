#ifndef PLAYERACTIONS_H
#define PLAYERACTIONS_H

#include "match/Distance.h"

class Match;
class Player;

class PlayerAction {
	public:
		virtual ~PlayerAction() { }
		virtual void applyPlayerAction(Match& match, Player& p, double time) = 0;
		virtual std::string getDescription() const = 0;
};

class IdlePA : public PlayerAction {
	public:
		void applyPlayerAction(Match& match, Player& p, double time);
		std::string getDescription() const;
};

class RunToPA : public PlayerAction {
	public:
		RunToPA(const Common::Vector3& v);
		void applyPlayerAction(Match& match, Player& p, double time);
		std::string getDescription() const;
	private:
		Common::Vector3 mDiff;
};

class KickBallPA : public PlayerAction {
	public:
		// the vector length should be between 0 and 1,
		// 1 being the maximum power
		KickBallPA(const Common::Vector3& v, Player* passtgt = nullptr, bool absolute = false);
		void applyPlayerAction(Match& match, Player& p, double time);
		std::string getDescription() const;
	private:
		Common::Vector3 mDiff;
		Player* mPassTarget;
		bool mAbsolute;
};

class GrabBallPA : public PlayerAction {
	public:
		void applyPlayerAction(Match& match, Player& p, double time);
		std::string getDescription() const;
};

class TacklePA : public PlayerAction {
	public:
		TacklePA(const Common::Vector3& v);
		void applyPlayerAction(Match& match, Player& p, double time);
		std::string getDescription() const;
	private:
		Common::Vector3 mDiff;
};

class JumpToPA : public PlayerAction {
	public:
		JumpToPA(const Common::Vector3& v);
		void applyPlayerAction(Match& match, Player& p, double time);
		std::string getDescription() const;
	private:
		Common::Vector3 mDiff;
};

#endif

