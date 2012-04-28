#include "Match.h"
#include "PlayerActions.h"
#include "RefereeActions.h"

Match::Match()
	: mTime(0),
	mMatchHalf(MatchHalf::NotStarted),
	mPlayState(PlayState::OutKickoff),
	mPitch(Pitch(50.0f, 100.0f)),
	mAICountdown(0.1f)
{
	static const int numPlayers = 11;

	for(int j = 0; j < 2; j++) {
		mTeams[j] = std::shared_ptr<Team>(new Team(this, j == 0));
		for(int i = 0; i < numPlayers; i++) {
			mTeams[j]->addPlayer(std::shared_ptr<Player>(new Player(this, mTeams[j].get())));
		}
	}
	mReferee.setMatch(this);
	mBall = std::shared_ptr<Ball>(new Ball(this));
}

const Player* Match::getPlayer(unsigned int team, unsigned int idx) const
{
	if(team > 2)
		return nullptr;
	return mTeams[team]->getPlayer(idx);
}

const Ball* Match::getBall() const
{
	return mBall.get();
}

void Match::update(double time)
{
	mAICountdown.doCountdown(time);
	if(mAICountdown.checkAndRewind()) {
		mCachedActions.clear();
	}
	for(auto& t : mTeams) {
		for(auto& p : t->getPlayers()) {
			auto cache = mCachedActions.find(p);
			if(cache == mCachedActions.end()) {
				std::shared_ptr<PlayerAction> a(p->act());
				applyPlayerAction(a, p, time);
				mCachedActions.insert(std::make_pair(p, a));
			}
			else {
				applyPlayerAction(cache->second, p, time);
			}
		}
	}

	updateReferee(time);
}

MatchHalf Match::getMatchHalf() const
{
	return mMatchHalf;
}

PlayState Match::getPlayState() const
{
	return mPlayState;
}

bool Match::matchOver() const
{
	return mMatchHalf == MatchHalf::Finished;
}

void Match::applyPlayerAction(const std::shared_ptr<PlayerAction> a, const std::shared_ptr<Player> p, double time)
{
	a->applyPlayerAction(*this, *p.get(), time);
}

void Match::updateReferee(double time)
{
	std::shared_ptr<RefereeAction> a(mReferee.act());
	a->applyRefereeAction(*this, mReferee, time);
}

AbsVector3 Match::convertRelativeToAbsoluteVector(const RelVector3& v) const
{
	return AbsVector3(v.v.x * mPitch.getWidth() * 0.5f, v.v.y * mPitch.getHeight() * 0.5f, v.v.z);
}

float Match::getPitchWidth() const
{
	return mPitch.getWidth();
}

float Match::getPitchHeight() const
{
	return mPitch.getHeight();
}


