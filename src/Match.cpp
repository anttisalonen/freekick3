#include "Match.h"
#include "PlayerActions.h"
#include "RefereeActions.h"

Match::Match()
	: mTime(0),
	mMatchHalf(MatchHalf::NotStarted),
	mPlayState(PlayState::OutKickoff)
{
	static const int numPlayers = 11;

	for(int j = 0; j < 2; j++) {
		for(int i = 0; i < numPlayers; i++) {
			mPlayers[j].push_back(std::shared_ptr<Player>(new Player(this)));
		}
	}
	mReferee.setMatch(this);
}

const Player* Match::getPlayer(unsigned int team, unsigned int idx) const
{
	if(team > 2)
		return nullptr;
	if(idx > mPlayers[team].size())
		return nullptr;
	return mPlayers[team][idx].get();
}

const Ball* Match::getBall() const
{
	return &mBall;
}

void Match::update(double time)
{
	for(auto& t : mPlayers) {
		for(auto& p : t) {
			std::shared_ptr<PlayerAction> a(p->act());
			applyPlayerAction(a, p, time);
		}
	}

	updateReferee(time);
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

