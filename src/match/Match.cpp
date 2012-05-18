#include <string>
#include <stdexcept>

#include "match/Match.h"
#include "match/Team.h"
#include "match/MatchHelpers.h"
#include "match/PlayerActions.h"
#include "match/RefereeActions.h"

Match::Match(const Soccer::Match& m)
	: Soccer::Match(m),
	mTime(0),
	mTimeAccelerationConstant(30),
	mMatchHalf(MatchHalf::NotStarted),
	mPlayState(PlayState::OutKickoff),
	mPitch(Pitch(68.0f, 105.0f))
{
	static const int numPlayers = 11;

	mScore[0] = mScore[1] = 0;

	for(int j = 0; j < 2; j++) {
		mTeams[j] = std::shared_ptr<Team>(new Team(this, *m.getTeam(j), j == 0));
		for(int i = 0; i < numPlayers; i++) {
			mTeams[j]->addPlayer(*m.getTeam(j)->getPlayer(i));
		}
	}
	mReferee.setMatch(this);
	mBall = std::shared_ptr<Ball>(new Ball(this));
}

Team* Match::getTeam(unsigned int team)
{
	if(team > 2)
		throw std::runtime_error("Invalid index when getting team");
	return mTeams[team].get();
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

const Referee* Match::getReferee() const
{
	return &mReferee;
}

void Match::update(double time)
{
	mBall->update(time);
	for(auto& t : mTeams) {
		t->act(time);
		for(auto& p : t->getPlayers()) {
			std::shared_ptr<PlayerAction> a(p->act(time));
			applyPlayerAction(a, p, time);
			p->update(time);
		}
	}

	updateReferee(time);
	updateTime(time);
}

MatchHalf Match::getMatchHalf() const
{
	return mMatchHalf;
}

void Match::setMatchHalf(MatchHalf h)
{
	if(h == mMatchHalf)
		return;

	std::cout << "Match half is now " << h << "\n";
	mMatchHalf = h;
	mPlayState = PlayState::OutKickoff;
	for(int i = 0; i < 2; i++)
		mTeams[i]->matchHalfChanged(mMatchHalf);
	mReferee.matchHalfChanged(mMatchHalf);
	if(mMatchHalf == MatchHalf::HalfTimePauseEnd)
		mBall->setPosition(AbsVector3(0, 0, 0));
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
		case MatchHalf::HalfTimePauseBegin:
			str = "Half time pause (begin)"; break;
		case MatchHalf::HalfTimePauseEnd:
			str = "Half time pause (end)"; break;
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

int Match::kickBall(Player* p, const AbsVector3& v)
{
	if(MatchHelpers::canKickBall(*p) && mReferee.ballKicked(*p, v)) {
		int failpoints = 0;
		if(!(mBall->getVelocity().v.length() / 80.0f < p->getSkills().BallControl))
			failpoints++;
		{
			Vector3 pb = mBall->getPosition().v - p->getPosition().v;
			Vector3 pt = (mBall->getPosition().v + v.v) - p->getPosition().v;
			if(pt.dot(pb) < 0.0f) {
				failpoints += 2;
			}
		}

		if(failpoints == 0)
			mBall->setVelocity(v);
		else
			mBall->addVelocity(AbsVector3(v.v.normalized() * (5.0f / failpoints)));
		mBall->kicked(p);
		p->ballKicked();
		for(auto t : mTeams)
			t->ballKicked(p);
		return failpoints;
	}
	else {
		return -1;
	}
}

double Match::getRollInertiaFactor() const
{
	// higher makes ball slower
	return 0.97;
}

double Match::getAirViscosityFactor() const
{
	// higher makes ball slower
	return 0.7;
}

void Match::addGoal(bool forFirst)
{
	mScore[forFirst ? 0 : 1]++;
}

int Match::getScore(bool first) const
{
	return mScore[first ? 0 : 1];
}

bool Match::grabBall(Player* p)
{
	if(p->isGoalkeeper() && !mBall->grabbed() && MatchEntity::distanceBetween(*p, *mBall) < 1.5f &&
			!MatchHelpers::myTeamInControl(*p) && mReferee.ballGrabbed(*p)) {
		mBall->grab(p);
		return true;
	}
	else {
		std::cout << "Can't grab the ball.\n";
		return false;
	}
}

void Match::updateTime(double time)
{
	if(playing(mMatchHalf) && playing(mPlayState) && !mBall->grabbed()) {
		mTime += time * mTimeAccelerationConstant / 60.0f;
		if(mTime >= 45.0f && (fabs(mBall->getPosition().v.y) < 20.0f || mTime > 50.5f)) {
			setMatchHalf(mMatchHalf == MatchHalf::FirstHalf ?
					MatchHalf::HalfTimePauseBegin : MatchHalf::Finished);
			mTime = 0.0f;
		}
	}
}

double Match::getTime() const
{
	return mTime;
}


