#ifndef REFEREE_H
#define REFEREE_H

#include <boost/shared_ptr.hpp>

#include "match/MatchEntity.h"
#include "match/Distance.h"
#include "match/Clock.h"

class Match;
class RefereeAction;
class Player;

enum class MatchHalf;

enum class BallOutStatus;

class Referee {
	public:
		Referee();
		void setMatch(Match* m);
		boost::shared_ptr<RefereeAction> act(double time);
		bool canKickBall(const Player& p) const;
		void ballKicked(const Player& p);
		bool isFirstTeamInControl() const;
		void ballGrabbed(const Player& p);
		void matchHalfChanged(MatchHalf m);
		const Player* getPlayerInControl() const;
		void playerTackled(const Player& tackled, const Player& tacklee);

	private:
		bool allPlayersOnOwnSideAndReady() const;
		void ballTouched(const Player& p);
		bool firstTeamAttacksUp() const;
		BallOutStatus getBallOutStatus() const;
		void addPenaltyShootoutResult();
		boost::shared_ptr<RefereeAction> setOutOfPlay();
		boost::shared_ptr<RefereeAction> setFoulRestart();

		Match* mMatch;
		bool mFirstTeamInControl;
		AbsVector3 mRestartPosition;
		Countdown mOutOfPlayClock;
		Countdown mWaitForResumeClock;
		Countdown mWaitForPenaltyShot;
		const Player* mPlayerInControl;
		int mFouledTeam;
		AbsVector3 mFoulPosition;
		const Player* mRestartedPlayer;
};

#endif

