#ifndef REFEREE_H
#define REFEREE_H

#include <memory>

#include "MatchEntity.h"
#include "Distance.h"
#include "Clock.h"

class Match;
class RefereeAction;
class Player;

class Referee {
	public:
		Referee();
		void setMatch(Match* m);
		std::shared_ptr<RefereeAction> act(double time);
		bool ballKicked(const Player& p, const AbsVector3& vel);
		bool isFirstTeamInControl() const;
	private:
		bool allPlayersOnOwnSideAndReady() const;
		bool onPitch(const MatchEntity& m) const;
		std::shared_ptr<RefereeAction> setOutOfPlay();
		Match* mMatch;
		bool mFirstTeamInControl;
		AbsVector3 mRestartPosition;
		Countdown mOutOfPlayClock;
		Countdown mWaitForResumeClock;
};

#endif

