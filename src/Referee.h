#ifndef REFEREE_H
#define REFEREE_H

#include <memory>

#include "MatchEntity.h"
#include "Distance.h"

class Match;
class RefereeAction;
class Player;

class Referee {
	public:
		Referee();
		void setMatch(Match* m);
		std::shared_ptr<RefereeAction> act();
		bool ballKicked(const Player& p, const AbsVector3& vel);
	private:
		bool allPlayersOnOwnSideAndReady() const;
		bool onPitch(const MatchEntity& m) const;
		Match* mMatch;
};

#endif

