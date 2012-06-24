#ifndef MATCH_H
#define MATCH_H

#include <iostream>
#include <vector>
#include <map>

#include "soccer/Match.h"

#include "match/Clock.h"
#include "match/Pitch.h"
#include "match/Team.h"
#include "match/Player.h"
#include "match/Ball.h"
#include "match/Referee.h"

enum class MatchHalf {
	NotStarted,
	FirstHalf,
	HalfTimePauseBegin,
	HalfTimePauseEnd,
	SecondHalf,
	Finished
};

std::ostream& operator<<(std::ostream& out, const MatchHalf& m);
bool playing(MatchHalf h);

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

std::ostream& operator<<(std::ostream& out, const PlayState& m);
bool playing(PlayState h);

class Match : public Soccer::Match {
	public:
		Match(const Soccer::Match& m, double matchtime);
		Team* getTeam(unsigned int team);
		const Team* getTeam(unsigned int team) const;
		const Player* getPlayer(unsigned int team, unsigned int idx) const;
		Player* getPlayer(unsigned int team, unsigned int idx);
		const Ball* getBall() const;
		Ball* getBall();
		const Referee* getReferee() const;
		void update(double time);
		bool matchOver() const;
		MatchHalf getMatchHalf() const;
		void setMatchHalf(MatchHalf h);
		void setPlayState(PlayState h);
		PlayState getPlayState() const;
		AbsVector3 convertRelativeToAbsoluteVector(const RelVector3& v) const;
		RelVector3 convertAbsoluteToRelativeVector(const AbsVector3& v) const;
		float getPitchWidth() const;
		float getPitchHeight() const;
		int kickBall(Player* p, const AbsVector3& v);
		double getRollInertiaFactor() const;
		double getAirViscosityFactor() const;
		void addGoal(bool forFirst);
		int getScore(bool first) const;
		bool grabBall(Player* p);
		double getTime() const;
	private:
		void applyPlayerAction(const boost::shared_ptr<PlayerAction> a,
				const boost::shared_ptr<Player> p, double time);
		void updateReferee(double time);
		void updateTime(double time);
		boost::shared_ptr<Team> mTeams[2];
		boost::shared_ptr<Ball> mBall;
		std::map<boost::shared_ptr<Player>, boost::shared_ptr<PlayerAction>> mCachedActions;
		Referee mReferee;
		double mTime;
		double mTimeAccelerationConstant;
		MatchHalf mMatchHalf;
		PlayState mPlayState;
		Pitch mPitch;
		int mScore[2];
};

#endif

