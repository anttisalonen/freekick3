#include <stdlib.h>

#include <stdexcept>

#include "common/Math.h"

#include "match/MatchHelpers.h"
#include "match/Referee.h"
#include "match/RefereeActions.h"

Referee::Referee()
	: mMatch(nullptr),
	mFirstTeamInControl(true),
	mOutOfPlayClock(1.0f),
	mWaitForResumeClock(0.1f),
	mPlayerInControl(nullptr),
	mFouledTeam(0),
	mRestartedPlayer(nullptr)
{
}

void Referee::setMatch(Match* m)
{
	mMatch = m;
}

boost::shared_ptr<RefereeAction> Referee::act(double time)
{
	switch(mMatch->getMatchHalf()) {
		case MatchHalf::NotStarted:
		case MatchHalf::HalfTimePauseEnd:
			if(allPlayersOnOwnSideAndReady()) {
				mFirstTeamInControl = mMatch->getMatchHalf() == MatchHalf::NotStarted;
				if(mFirstTeamInControl)
					return boost::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::FirstHalf));
				else
					return boost::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::SecondHalf));
			}
			break;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			if(!mOutOfPlayClock.running()) {
				if(mMatch->getPlayState() == PlayState::InPlay) {
					if(!mWaitForResumeClock.running()) {
						if(mFouledTeam != 0) {
							mOutOfPlayClock.rewind();
							boost::shared_ptr<RefereeAction> act = setFoulRestart();
							return act;
						}
						if(!MatchHelpers::onPitch(*mMatch->getBall())) {
							boost::shared_ptr<RefereeAction> act = setOutOfPlay();
							if(act) {
								mOutOfPlayClock.rewind();
								mFouledTeam = 0;
								return act;
							}
						}
					}
					else {
						mWaitForResumeClock.doCountdown(time);
						mWaitForResumeClock.check();
					}
				}
			}
			else {
				mOutOfPlayClock.doCountdown(time);
				if(mOutOfPlayClock.check()) {
					mMatch->getBall()->setPosition(mRestartPosition);
					mMatch->getBall()->setVelocity(AbsVector3());
				}
			}
			break;

		case MatchHalf::HalfTimePauseBegin:
		case MatchHalf::Finished:
			break;
	}
	mFouledTeam = 0;
	return boost::shared_ptr<RefereeAction>(new IdleRA());
}

bool Referee::allPlayersOnOwnSideAndReady() const
{
	for(int i = 0; i < 2; i++) {
		for(auto& p : mMatch->getTeam(i)->getPlayers()) {
			if(!MatchHelpers::onOwnSideAndReady(*p))
				return false;
		}
	}
	return true;
}

bool Referee::canKickBall(const Player& p) const
{
	switch(mMatch->getMatchHalf()) {
		case MatchHalf::NotStarted:
		case MatchHalf::HalfTimePauseBegin:
		case MatchHalf::HalfTimePauseEnd:
			return true;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			switch(mMatch->getPlayState()) {
				case PlayState::InPlay:
					return true;

				default:
					return MatchHelpers::playersPositionedForRestart(*p.getMatch(), p);
			}

		case MatchHalf::Finished:
			return true;
	}
	return false;
}

void Referee::ballKicked(const Player& p)
{
	if(playing(mMatch->getMatchHalf())) {
		ballTouched(p);

		std::cout << "Ball kicked by " << p.getName() << "\n";
		if(!playing(mMatch->getPlayState())) {
			if(p.getTeam()->isFirst() == mFirstTeamInControl) {
				mMatch->setPlayState(PlayState::InPlay);
				mWaitForResumeClock.rewind();
				mRestartedPlayer = &p;
				std::cout << "Restart by " << p.getName() << "\n";
			}
		}
		else {
			if(&p == mRestartedPlayer && !mOutOfPlayClock.running() && !mWaitForResumeClock.running()) {
				mRestartPosition = p.getMatch()->getBall()->getPosition();
				mRestartPosition.v.z = 0.0f;
				mFirstTeamInControl = !p.getTeam()->isFirst();
				mPlayerInControl = nullptr;
				std::cout << "Double touch by " << p.getName() << " - restart position: " << mRestartPosition.v << " by " << mFirstTeamInControl << "\n";
				mOutOfPlayClock.rewind();
				mMatch->setPlayState(PlayState::OutIndirectFreekick);
			}
			mRestartedPlayer = nullptr;
		}
	}
}

boost::shared_ptr<RefereeAction> Referee::setOutOfPlay()
{
	RelVector3 bp(mMatch->convertAbsoluteToRelativeVector(mMatch->getBall()->getPosition()));
	RelVector3 br(mMatch->convertAbsoluteToRelativeVector(AbsVector3(BALL_RADIUS, BALL_RADIUS, BALL_RADIUS)));
	if(fabs(bp.v.x + br.v.x) > 1.0f) {
		mRestartPosition = mMatch->getBall()->getPosition();
		mRestartPosition.v.x = mMatch->getPitchWidth() * 0.5f;
		if(bp.v.x < 0.0f)
			mRestartPosition.v.x = -mRestartPosition.v.x;
		mRestartPosition.v.z = 0.0f;
		mFirstTeamInControl = !mFirstTeamInControl;
		mPlayerInControl = nullptr;
		return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutThrowin));
	}
	if(fabs(bp.v.y + br.v.y) > 1.0f) {
		if(fabs(mMatch->getBall()->getPosition().v.x) < GOAL_WIDTH_2 &&
				mMatch->getBall()->getPosition().v.z < GOAL_HEIGHT) {
			// goal
			mRestartPosition.v.x = 0.0f;
			mRestartPosition.v.y = 0.0f;
			mRestartPosition.v.z = 0.0f;
			bool firstscores;
			if(bp.v.y > 1.0f) {
				firstscores = mMatch->getMatchHalf() == MatchHalf::FirstHalf;
			}
			else {
				firstscores = mMatch->getMatchHalf() != MatchHalf::FirstHalf;
			}
			mMatch->addGoal(firstscores);
			mFirstTeamInControl = !firstscores;
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutKickoff));
		}
		if((((bp.v.y < 0.0f) == mFirstTeamInControl) && (mMatch->getMatchHalf() == MatchHalf::FirstHalf)) ||
		   (((bp.v.y < 0.0f) != mFirstTeamInControl) && (mMatch->getMatchHalf() == MatchHalf::SecondHalf))) {
			if(bp.v.x == 0.0f)
				bp.v.x = 1.0f;
			if(bp.v.y == 0.0f)
				bp.v.y = 1.0f;
			mRestartPosition.v.x = Common::signum(bp.v.x) * mMatch->getPitchWidth() * 0.5f;
			mRestartPosition.v.y = Common::signum(bp.v.y) * mMatch->getPitchHeight() * 0.5f;
			mRestartPosition.v.z = 0.0f;
			mFirstTeamInControl = !mFirstTeamInControl;
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutCornerkick));
		}
		else {
			mRestartPosition.v.x = 9.16f;
			if(bp.v.x < 0) {
				mRestartPosition.v.x = -mRestartPosition.v.x;
			}
			mRestartPosition.v.y = mMatch->getPitchHeight() * 0.5f - 5.5f;
			if(bp.v.y < 0) {
				mRestartPosition.v.y = -mRestartPosition.v.y;
			}
			mRestartPosition.v.z = 0.0f;
			mFirstTeamInControl = !mFirstTeamInControl;
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutGoalkick));
		}
	}
	return boost::shared_ptr<RefereeAction>();
}

boost::shared_ptr<RefereeAction> Referee::setFoulRestart()
{
	assert(mFouledTeam != 0);
	mFirstTeamInControl = mFouledTeam == 2;
	mFouledTeam = 0;

	mRestartPosition = mFoulPosition;
	mRestartPosition.v.z = 0.0f;
	mPlayerInControl = nullptr;

	int pen = MatchHelpers::inPenaltyArea(*mMatch, mRestartPosition);
	if(pen) {
		if((MatchHelpers::attacksUp(*mMatch->getTeam(0)) == mFirstTeamInControl && pen ==  1) ||
		   (MatchHelpers::attacksUp(*mMatch->getTeam(0)) != mFirstTeamInControl && pen == -1)) {
			bool up = pen == 1;
			mRestartPosition.v.x = 0.0f;
			mRestartPosition.v.y = (up ? 1.0f : -1.0f) * (mMatch->getPitchHeight() * 0.5f - 11.00f);
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutPenaltykick));
		}
	}
	return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutDirectFreekick));
}

bool Referee::isFirstTeamInControl() const
{
	return mFirstTeamInControl;
}

void Referee::ballGrabbed(const Player& p)
{
	mFirstTeamInControl = p.getTeam()->isFirst();
	mPlayerInControl = &p;
}

void Referee::matchHalfChanged(MatchHalf m)
{
	mPlayerInControl = nullptr;
	if(m == MatchHalf::HalfTimePauseEnd) {
		mFirstTeamInControl = false;
	}
}

const Player* Referee::getPlayerInControl() const
{
	return mPlayerInControl;
}

void Referee::ballTouched(const Player& p)
{
	mFirstTeamInControl = p.getTeam()->isFirst();
	mPlayerInControl = &p;
}

void Referee::playerTackled(const Player& tackled, const Player& tacklee)
{
	if((mFirstTeamInControl == tackled.getTeam()->isFirst()) ||
			MatchEntity::distanceBetween(tacklee, *mMatch->getBall()) > MAX_KICK_DISTANCE) {
		if(tackled.getTeam()->isFirst())
			mFouledTeam = 2;
		else
			mFouledTeam = 1;

		mFoulPosition = tackled.getPosition();
	}
}


