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
		mTeams[j] = std::shared_ptr<Team>(new Team(this, j == 0));
		for(int i = 0; i < numPlayers; i++) {
			mTeams[j]->addPlayer(std::shared_ptr<Player>(new Player(this, mTeams[j].get())));
		}
	}
	mReferee.setMatch(this);
}

const Player* Match::getPlayer(unsigned int team, unsigned int idx) const
{
	if(team > 2)
		return nullptr;
	return mTeams[team]->getPlayer(idx);
}

const Ball* Match::getBall() const
{
	return &mBall;
}

void Match::update(double time)
{
	for(auto& t : mTeams) {
		for(auto& p : t->getPlayers()) {
			std::shared_ptr<PlayerAction> a(p->act());
			applyPlayerAction(a, p, time);
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
	return AbsVector3(v.v.x * 25.0f, v.v.y * 50.0f, v.v.z);
}


