#ifndef MATCH_H
#define MATCH_H

#include <vector>

#include "Pitch.h"
#include "Team.h"
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
		MatchHalf getMatchHalf() const;
		PlayState getPlayState() const;
		AbsVector3 convertRelativeToAbsoluteVector(const RelVector3& v) const;
		float getPitchWidth() const;
		float getPitchHeight() const;
	private:
		void applyPlayerAction(const std::shared_ptr<PlayerAction> a,
				const std::shared_ptr<Player> p, double time);
		void updateReferee(double time);
		std::shared_ptr<Team> mTeams[2];
		std::shared_ptr<Ball> mBall;
		Referee mReferee;
		double mTime;
		MatchHalf mMatchHalf;
		PlayState mPlayState;
		Pitch mPitch;
};

#endif

