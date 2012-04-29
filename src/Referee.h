#ifndef REFEREE_H
#define REFEREE_H

#include <memory>

#include "MatchEntity.h"

class Match;
class RefereeAction;

class Referee {
	public:
		Referee();
		void setMatch(Match* m);
		std::shared_ptr<RefereeAction> act();
	private:
		bool allPlayersOnOwnSideAndReady() const;
		bool onPitch(const MatchEntity& m) const;
		Match* mMatch;
};

#endif

