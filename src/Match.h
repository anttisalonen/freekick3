#ifndef MATCH_H
#define MATCH_H

#include <vector>

#include "Player.h"
#include "Ball.h"
#include "Referee.h"

enum class MatchHalf {
	NotStarted,
	FirstHalf,
	HalfTimePause,
	SecondHalf,
	Finished
};

enum class PlayState {
	InPlay,
	OutKickoff,
	OutThrowin,
	OutGoalkick,
	OutCornerkick,
	OutIndirectFreekick,
	OutDirectFreekick,
	OutPenaltykick,
	OutDroppedball
};

class Match {
	public:
		Match();
		const Player* getPlayer(unsigned int team, unsigned int idx) const;
		const Ball* getBall() const;
		void update(double time);
		bool matchOver() const;
	private:
		void applyPlayerAction(const std::shared_ptr<PlayerAction> a,
				const std::shared_ptr<Player> p, double time);
		void updateReferee(double time);
		std::vector<std::shared_ptr<Player>> mPlayers[2];
		Ball mBall;
		Referee mReferee;
		double mTime;
		MatchHalf mMatchHalf;
		PlayState mPlayState;
};

#endif

