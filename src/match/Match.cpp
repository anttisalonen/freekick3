#include <string>
#include <stdexcept>

#include "match/Match.h"
#include "match/Team.h"
#include "match/MatchHelpers.h"
#include "match/PlayerActions.h"
#include "match/RefereeActions.h"

#define TACKLE_DISTANCE 1.0f

Match::Match(const Soccer::Match& m, double matchtime)
	: Soccer::Match(m),
	mTime(0),
	mTimeAccelerationConstant(90.0f / matchtime),
	mMatchHalf(MatchHalf::NotStarted),
	mPlayState(PlayState::OutKickoff),
	mPitch(Pitch(68.0f, 105.0f))
{
	static const unsigned int numPlayers = 11;
	assert(matchtime);

	mScore[0] = mScore[1] = 0;

	for(int j = 0; j < 2; j++) {
		mTeams[j] = boost::shared_ptr<Team>(new Team(this, *m.getTeam(j), j == 0));
		unsigned int i = 0;
		for(auto n : mTeams[j]->getTactics().mTactics) {
			boost::shared_ptr<Soccer::Player> pl(m.getTeam(j)->getPlayerById(n.first));
			if(!pl) {
				std::cerr << "Warning: Team " << mTeams[j]->getName() << " is missing tactics for player ID " << n.first << ".\n";
				continue;
			}
			mTeams[j]->addPlayer(*pl);
			i++;
			if(i >= numPlayers)
				break;
		}

		if(mTeams[j]->getNumPlayers() < numPlayers) {
			std::cerr << "Warning: Team " << mTeams[j]->getName() << " doesn't have enough players.\n";
		}
	}
	mReferee.setMatch(this);
	mBall = boost::shared_ptr<Ball>(new Ball(this));
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

	for(int i = 0; i < 2; i++) {
		int j = i == 0 ? 1 : 0;
		auto& t = mTeams[i];

		t->act(time);
		for(auto& p : t->getPlayers()) {
			boost::shared_ptr<PlayerAction> a(p->act(time));
			applyPlayerAction(a.get(), p, time);
			if(p->tackling() && MatchHelpers::canKickBall(*p)) {
				if(!p->getVelocity().v.null()) {
					KickBallPA pa(AbsVector3(p->getVelocity().v.normalized() * 0.3f),
							nullptr, false);
					applyPlayerAction(&pa, p, time);
				}
			}
			p->update(time);
			for(auto& p2 : mTeams[j]->getPlayers()) {
				if(p2->tackling() && p->standing() && !p->isAirborne()) {
					float dist = MatchEntity::distanceBetween(*p, *p2);
					if(dist < TACKLE_DISTANCE) {
						std::cout << "Tackled player\n";
						p->setTackled();
						mReferee.playerTackled(*p, *p2);
					}
				}
			}
		}
	}

	const Player* collided = mBall->checkPlayerCollisions();
	if(collided && mReferee.canKickBall(*collided)) {
		mReferee.ballKicked(*collided);
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

void Match::applyPlayerAction(PlayerAction* a, const boost::shared_ptr<Player> p, double time)
{
	a->applyPlayerAction(*this, *p.get(), time);
}

void Match::updateReferee(double time)
{
	boost::shared_ptr<RefereeAction> a(mReferee.act(time));
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
	if(MatchHelpers::canKickBall(*p) && mReferee.canKickBall(*p)) {
		int failpoints = 0;

		if(playing(getPlayState())) {
			if(!(mBall->getVelocity().v.length() / 80.0f < p->getSkills().BallControl)) {
				failpoints++;
				if(MatchEntity::distanceBetween(*mBall, *p) < MAX_KICK_DISTANCE * 0.5f) {
					failpoints++;
				}
			}

			if(!MatchHelpers::goodKickingPosition(*p, v)) {
				failpoints += 2;
			}

			p->ballKicked();
			if(failpoints == 4) {
				return -1;
			}
		}

		if(failpoints == 0)
			mBall->setVelocity(v);
		else
			mBall->addVelocity(AbsVector3(v.v.normalized() * (5.0f / failpoints)));
		mBall->kicked(p);
		mReferee.ballKicked(*p);
		for(auto t : mTeams)
			t->ballKicked(p);
		return failpoints;
	}
	else {
		p->ballKicked();
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
	return 0.4;
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
	if(MatchHelpers::canGrabBall(*p)) {
		mBall->grab(p);
		mReferee.ballGrabbed(*p);
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
		mTime += time * mTimeAccelerationConstant;
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


