#ifndef REFEREE_H
#define REFEREE_H

#include <memory>

#include "match/MatchEntity.h"
#include "match/Distance.h"
#include "match/Clock.h"

class Match;
class RefereeAction;
class Player;

enum class MatchHalf;

class Referee {
	public:
		Referee();
		void setMatch(Match* m);
		std::shared_ptr<RefereeAction> act(double time);
		bool ballKicked(const Player& p, const AbsVector3& vel);
		bool isFirstTeamInControl() const;
		bool ballGrabbed(const Player& p);
		void matchHalfChanged(MatchHalf m);
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

