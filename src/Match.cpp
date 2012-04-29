#include <string>
#include <stdexcept>

#include "Match.h"
#include "PlayerActions.h"
#include "RefereeActions.h"

Match::Match()
	: mTime(0),
	mMatchHalf(MatchHalf::NotStarted),
	mPlayState(PlayState::OutKickoff),
	mPitch(Pitch(50.0f, 100.0f)),
	mRefCountdown(0.3f)
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

const Team* Match::getTeam(unsigned int team) const
{
	if(team > 2)
		throw std::runtime_error("Invalid index when getting team");
	return mTeams[team].get();
}

const Player* Match::getPlayer(unsigned int team, unsigned int idx) const
{
	if(team > 2)
		throw std::runtime_error("Invalid index when getting player");
	return mTeams[team]->getPlayer(idx);
}

Player* Match::getPlayer(unsigned int team, unsigned int idx)
{
	if(team > 2)
		throw std::runtime_error("Invalid index when getting player");
	return mTeams[team]->getPlayer(idx);
}

const Ball* Match::getBall() const
{
	return mBall.get();
}

Ball* Match::getBall()
{
	return mBall.get();
}

void Match::update(double time)
{
	mBall->update(time);
	for(auto& t : mTeams) {
		for(auto& p : t->getPlayers()) {
			std::shared_ptr<PlayerAction> a(p->act(time));
			applyPlayerAction(a, p, time);
			p->update(time);
		}
	}

	mRefCountdown.doCountdown(time);
	if(mRefCountdown.checkAndRewind()) {
		updateReferee(time);
	}
}

MatchHalf Match::getMatchHalf() const
{
	return mMatchHalf;
}

void Match::setMatchHalf(MatchHalf h)
{
	std::cout << "Match half is now " << h << "\n";
	mMatchHalf = h;
	mPlayState = PlayState::OutKickoff;
}

void Match::setPlayState(PlayState h)
{
	std::cout << "Play state is now " << h << "\n";
	mPlayState = h;
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
	std::shared_ptr<RefereeAction> a(mReferee.act(time));
	a->applyRefereeAction(*this, mReferee, time);
}

AbsVector3 Match::convertRelativeToAbsoluteVector(const RelVector3& v) const
{
	return AbsVector3(v.v.x * mPitch.getWidth() * 0.5f, v.v.y * mPitch.getHeight() * 0.5f, v.v.z);
}

RelVector3 Match::convertAbsoluteToRelativeVector(const AbsVector3& v) const
{
	return RelVector3(v.v.x / mPitch.getWidth() * 2.0f, v.v.y / mPitch.getHeight() * 2.0f, v.v.z);
}

float Match::getPitchWidth() const
{
	return mPitch.getWidth();
}

float Match::getPitchHeight() const
{
	return mPitch.getHeight();
}

std::ostream& operator<<(std::ostream& out, const MatchHalf& m)
{
	const char* str;
	switch(m) {
		case MatchHalf::NotStarted:
			str = "Not started"; break;
		case MatchHalf::FirstHalf:
			str = "First half"; break;
		case MatchHalf::HalfTimePause:
			str = "Half time pause"; break;
		case MatchHalf::SecondHalf:
			str = "Second half"; break;
		case MatchHalf::Finished:
			str = "Finished"; break;
	}
	out << str;
	return out;
}

bool playing(MatchHalf h)
{
	return h == MatchHalf::FirstHalf || h == MatchHalf::SecondHalf;
}

std::ostream& operator<<(std::ostream& out, const PlayState& m)
{
	const char* str;
	switch(m) {
		case PlayState::InPlay:
			str = "In play"; break;
		case PlayState::OutKickoff:
			str = "Kick off"; break;
		case PlayState::OutThrowin:
			str = "Throw in"; break;
		case PlayState::OutGoalkick:
			str = "Goal kick"; break;
		case PlayState::OutCornerkick:
			str = "Corner kick"; break;
		case PlayState::OutIndirectFreekick:
			str = "Indirect free kick"; break;
		case PlayState::OutDirectFreekick:
			str = "Direct free kick"; break;
		case PlayState::OutPenaltykick:
			str = "Penalty"; break;
		case PlayState::OutDroppedball:
			str = "Dropped ball"; break;
	}
	out << str;
	return out;
}

bool playing(PlayState h)
{
	return h == PlayState::InPlay;
}

bool Match::kickBall(const Player& p, const AbsVector3& v)
{
	if(mReferee.ballKicked(p, v)) {
		mBall->setVelocity(v);
		return true;
	}
	else {
		return false;
	}
}

double Match::getRollInertiaFactor() const
{
	return 0.97;
}

