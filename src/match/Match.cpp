#include <string>
#include <stdexcept>

#include "common/Vector3.h"

#include "match/Match.h"
#include "match/Team.h"
#include "match/MatchHelpers.h"
#include "match/PlayerActions.h"
#include "match/RefereeActions.h"

#define TACKLE_DISTANCE 1.0f
#define PLAYER_RADIUS 0.6f

using Common::Vector3;

Match::Match(const Soccer::Match& m, double matchtime, bool extratime, bool penalties,
				bool awaygoals, int homeagg, int awayagg)
	: Soccer::Match(m),
	mTime(0),
	mTimeAccelerationConstant(90.0f / matchtime),
	mMatchHalf(MatchHalf::NotStarted),
	mPlayState(PlayState::OutKickoff),
	mPitch(Pitch(68.0f, 105.0f)),
	mGoalScorer(nullptr),
	mExtraTime(extratime),
	mPenalties(penalties),
	mAwayGoals(awaygoals),
	mHomeAgg(homeagg),
	mAwayAgg(awayagg)
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
				if(!p->getVelocity().null()) {
					KickBallPA pa(Vector3(p->getVelocity().normalized() * 0.3f),
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
				} else if(p->standing() && !p2->tackling() && !p2->isAirborne()) {
					checkPlayerPlayerCollision(p, p2);
				}
			}
		}
	}

	const Player* collided = mBall->checkPlayerCollisions();
	if(collided && mReferee.canKickBall(*collided)) {
		mReferee.ballKicked(*collided);
	}

	if(mMatchHalf == MatchHalf::PenaltyShootout) {
		if(mPenaltyShootout.isFinished()) {
			setMatchHalf(MatchHalf::Finished);
		}
	}

	updateReferee(time);
	updateTime(time);
}

void Match::checkPlayerPlayerCollision(boost::shared_ptr<Player> p, boost::shared_ptr<Player> p2)
{
	auto vec = MatchEntity::vectorFromTo(*p, *p2);
	float pen = PLAYER_RADIUS * 2.0f - vec.length();
	if(pen > 0.0f) {
		p->move(vec * -pen * 0.5f);
	}
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
	if(mMatchHalf == MatchHalf::HalfTimePauseEnd ||
			mMatchHalf == MatchHalf::FullTimePauseEnd ||
			mMatchHalf == MatchHalf::ExtraTimeSecondHalf) {
		mBall->setPosition(Vector3(0, 0, 0.4));
		mBall->setVelocity(Vector3(0, 0, 0));
	}
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

Vector3 Match::convertRelativeToAbsoluteVector(const RelVector3& v) const
{
	return Vector3(v.v.x * mPitch.getWidth() * 0.5f, v.v.y * mPitch.getHeight() * 0.5f, v.v.z);
}

RelVector3 Match::convertAbsoluteToRelativeVector(const Vector3& v) const
{
	return RelVector3(v.x / mPitch.getWidth() * 2.0f, v.y / mPitch.getHeight() * 2.0f, v.z);
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
		case MatchHalf::FullTimePauseBegin:
			str = "Full time pause (begin)"; break;
		case MatchHalf::FullTimePauseEnd:
			str = "Full time pause (end)"; break;
		case MatchHalf::ExtraTimeFirstHalf:
			str = "ET First half"; break;
		case MatchHalf::ExtraTimeSecondHalf:
			str = "ET Second half"; break;
		case MatchHalf::PenaltyShootout:
			str = "Penalty shootout"; break;
		case MatchHalf::Finished:
			str = "Finished"; break;
	}
	out << str;
	return out;
}

bool playing(MatchHalf h)
{
	return h == MatchHalf::FirstHalf || h == MatchHalf::SecondHalf ||
		h == MatchHalf::ExtraTimeFirstHalf || h == MatchHalf::ExtraTimeSecondHalf ||
		h == MatchHalf::PenaltyShootout;
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

int Match::kickBall(Player* p, const Vector3& v)
{
	if(MatchHelpers::canKickBall(*p) && mReferee.canKickBall(*p)) {
		int failpoints = 0;
		p->ballKicked();

		if(playing(getPlayState())) {
			if(!MatchHelpers::ballInHeadingHeight(*p) &&
					!(mBall->getVelocity().length() / 80.0f < p->getSkills().BallControl)) {
				failpoints++;
				if(MatchEntity::distanceBetween(*mBall, *p) < MAX_KICK_DISTANCE * 0.5f) {
					failpoints++;
				}
			}

			if(!MatchHelpers::goodKickingPosition(*p, v)) {
				failpoints += 2;
			}

			if(failpoints == 4) {
				return -1;
			}
		}

		Vector3 ballvel(v);

		if(getPlayState() == PlayState::OutThrowin) {
			Vector3 pos = mBall->getPosition();
			pos.z = 1.8f;
			mBall->setPosition(pos);
			ballvel.z += ballvel.length() * 0.5f;

			if(ballvel.length() > 20.0f) {
				ballvel.normalize();
				ballvel *= 20.0f;
			}
		}

		if(failpoints == 0)
			mBall->setVelocity(ballvel);
		else
			mBall->addVelocity(Vector3(ballvel * (1.0f / (failpoints + 3))));
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
	/* TODO: set penalty flag correctly */
	mGoalInfos[forFirst ? 0 : 1].push_back(GoalInfo(*this, false, mGoalScorer->getTeam()->isFirst() != forFirst));
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
		if(mMatchHalf != MatchHalf::PenaltyShootout) {
			mTime += time * mTimeAccelerationConstant;
		}
		if(mMatchHalf == MatchHalf::FirstHalf ||
				mMatchHalf == MatchHalf::SecondHalf) {
			if(mTime >= 45.0f && (fabs(mBall->getPosition().y) < 20.0f || mTime > 50.5f)) {
				if(mMatchHalf == MatchHalf::FirstHalf) {
					setMatchHalf(MatchHalf::HalfTimePauseBegin);
				} else {
					// Second half is finished
					bool tie = mAwayGoals ? mScore[0] == mAwayAgg && mScore[1] == mHomeAgg :
						mScore[0] == mScore[1];
					if((tie) && (mExtraTime || mPenalties)) {
						if(mExtraTime) {
							setMatchHalf(MatchHalf::FullTimePauseBegin);
						} else {
							setMatchHalf(MatchHalf::PenaltyShootout);
						}
					} else {
						setMatchHalf(MatchHalf::Finished);
					}
				}
				mTime = 0.0f;
			}
		} else if(mMatchHalf == MatchHalf::ExtraTimeFirstHalf ||
				mMatchHalf == MatchHalf::ExtraTimeSecondHalf) {
			if(mTime >= 15.0f && (fabs(mBall->getPosition().y) < 25.0f || mTime > 16.5f)) {
				if(mMatchHalf == MatchHalf::ExtraTimeFirstHalf) {
					setMatchHalf(MatchHalf::ExtraTimeSecondHalf);
				} else {
					bool tie = mAwayGoals ? mScore[0] == mAwayAgg && mScore[1] == mHomeAgg :
						mScore[0] == mScore[1];
					if(tie && mPenalties) {
						setMatchHalf(MatchHalf::PenaltyShootout);
					} else {
						setMatchHalf(MatchHalf::Finished);
					}
				}
				mTime = 0.0f;
			}
		}
	}
}

double Match::getTime() const
{
	return mTime;
}

void Match::setGoalScorer(const Player* p)
{
	mGoalScorer = p;
}

const Player* Match::getGoalScorer() const
{
	if(!mGoalScorer)
		throw std::runtime_error("Match::getGoalScorer() called without a goal scorer");
	return mGoalScorer;
}

const std::array<std::vector<GoalInfo>, 2>& Match::getGoalInfos() const
{
	return mGoalInfos;
}

const PenaltyShootout& Match::getPenaltyShootout() const
{
	return mPenaltyShootout;
}

void Match::addPenaltyShootoutShot(bool goal)
{
	mPenaltyShootout.addShot(goal);
}

bool Match::getAwayGoals() const
{
	return mAwayGoals;
}

int Match::getAggregateScore(bool first) const
{
	return first ? mHomeAgg + mScore[0] : mAwayAgg + mScore[1];
}


GoalInfo::GoalInfo(const Match& m, bool pen, bool own)
{
	const Player* scorer = m.getGoalScorer();
	mScorerName = scorer->getName();
	mShortScorerName = Soccer::Player::getShorterName(*scorer);

	if(own) {
		mScorerName += " (og)";
		mShortScorerName += " (og)";
	}

	if(pen) {
		mScorerName += "(pen.)";
		mShortScorerName += "(pen.)";
	}

	int min = m.getTime();

	switch(m.getMatchHalf()) {
		case MatchHalf::SecondHalf:
			if(min > 45) {
				mScoreTime = std::string("90 + ") + std::to_string(min - 45);
			}
			else {
				mScoreTime = std::to_string(min + 45);
			}
			break;

		case MatchHalf::ExtraTimeFirstHalf:
			if(min > 15) {
				mScoreTime = std::string("105 + ") + std::to_string(min - 15);
			}
			else {
				mScoreTime = std::to_string(min + 90);
			}
			break;

		case MatchHalf::ExtraTimeSecondHalf:
			if(min > 15) {
				mScoreTime = std::string("120 + ") + std::to_string(min - 15);
			}
			else {
				mScoreTime = std::to_string(min + 105);
			}
			break;

		default:
			if(min > 45) {
				mScoreTime = std::string("45 + ") + std::to_string(min - 45);
			}
			else {
				mScoreTime = std::to_string(min);
			}
			break;
	}
}

const std::string& GoalInfo::getScorerName() const
{
	return mScorerName;
}

const std::string& GoalInfo::getShortScorerName() const
{
	return mShortScorerName;
}

const std::string& GoalInfo::getScoreTime() const
{
	return mScoreTime;
}

PenaltyShootout::PenaltyShootout()
	: mFirstNext(true),
	mFinished(false),
	mRoundNumber(0)
{
	mGoals[0] = mGoals[1] = 0;
}

void PenaltyShootout::addShot(bool goal)
{
	static const unsigned int TotalRounds = 5;
	if(mFinished)
		return;

	if(goal)
		mGoals[mFirstNext ? 0 : 1]++;

	mFirstNext = !mFirstNext;
	if(mFirstNext)
		mRoundNumber++;

	// update finished flag
	std::cout << "Penalty shootout status: round " << mRoundNumber << ", "
		<< mGoals[0] << "-" << mGoals[1] << " - first next: " << mFirstNext << "\n";
	if(mGoals[0] != mGoals[1]) {
		if(mRoundNumber >= TotalRounds) {
			if(mFirstNext) {
				mFinished = true;
			}
		} else {
			unsigned int kicksLeft = TotalRounds - mRoundNumber;
			unsigned int losingTeamGoals  = mGoals[0] < mGoals[1] ? mGoals[0] : mGoals[1];
			unsigned int winningTeamGoals = mGoals[0] > mGoals[1] ? mGoals[0] : mGoals[1];

			if(losingTeamGoals == mGoals[0] && !mFirstNext) {
				kicksLeft--;
			}

			if(losingTeamGoals + kicksLeft < winningTeamGoals) {
				mFinished = true;
			}
		}
	}
}

bool PenaltyShootout::firstTeamKicksNext() const
{
	return mFirstNext;
}

int PenaltyShootout::getScore(bool first) const
{
	return mGoals[first ? 0 : 1];
}

unsigned int PenaltyShootout::getRoundNumber() const
{
	return mRoundNumber;
}

bool PenaltyShootout::isFinished() const
{
	return mFinished;
}


