#include <stdlib.h>

#include <stdexcept>

#include "common/Math.h"

#include "match/MatchHelpers.h"
#include "match/Referee.h"
#include "match/RefereeActions.h"

using Common::Vector3;

// #define DEBUG_CONTROLLING_TEAM

enum class BallOutStatus {
	Throwin,
	Goal,
	CornerKick,
	GoalKick,
	OnPitch,
};

Referee::Referee()
	: mMatch(nullptr),
	mFirstTeamInControl(true),
	mOutOfPlayClock(1.0f),
	mWaitForResumeClock(0.1f),
	mWaitForPenaltyShot(2.0f),
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
		case MatchHalf::FullTimePauseEnd:
			if(allPlayersOnOwnSideAndReady()) {
				mFirstTeamInControl = mMatch->getMatchHalf() == MatchHalf::NotStarted ||
					mMatch->getMatchHalf() == MatchHalf::FullTimePauseEnd;
#ifdef DEBUG_CONTROLLING_TEAM
				std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - match half\n";
#endif
				if(mFirstTeamInControl) {
					if(mMatch->getMatchHalf() == MatchHalf::NotStarted)
						return boost::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::FirstHalf));
					else
						return boost::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::ExtraTimeFirstHalf));
				}
				else {
					if(mMatch->getMatchHalf() == MatchHalf::HalfTimePauseEnd)
						return boost::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::SecondHalf));
					else
						return boost::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::ExtraTimeSecondHalf));
				}
			}
			break;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
		case MatchHalf::ExtraTimeFirstHalf:
		case MatchHalf::ExtraTimeSecondHalf:
			if(!mOutOfPlayClock.running()) {
				if(mMatch->getPlayState() == PlayState::InPlay) {
					if(!mWaitForResumeClock.running()) {
						if(mFouledTeam != 0) {
							mOutOfPlayClock.rewind();
							boost::shared_ptr<RefereeAction> a = setFoulRestart();
							return a;
						}
						if(!MatchHelpers::onPitch(*mMatch->getBall())) {
							boost::shared_ptr<RefereeAction> a = setOutOfPlay();
							if(a) {
								mOutOfPlayClock.rewind();
								return a;
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
					if(mMatch->getBall()->grabbed())
						mMatch->getBall()->drop();
					mMatch->getBall()->setPosition(mRestartPosition);
					mMatch->getBall()->setVelocity(Vector3());
				}
			}
			break;

		case MatchHalf::PenaltyShootout:
			if(!mOutOfPlayClock.running()) {
				if(mMatch->getPlayState() == PlayState::InPlay) {
					if(mMatch->getMatchHalf() == MatchHalf::PenaltyShootout) {
						if(!mWaitForPenaltyShot.running()) {
							mWaitForPenaltyShot.rewind();
						} else {
							mWaitForPenaltyShot.doCountdown(time);
							if(mWaitForPenaltyShot.check() ||
									mMatch->getBall()->grabbed() ||
									!MatchHelpers::onPitch(*mMatch->getBall())) {
								mRestartPosition.x = 0.0f;
								mRestartPosition.y = 1.0f * (mMatch->getPitchHeight() * 0.5f - 11.00f);
								mOutOfPlayClock.rewind();
								addPenaltyShootoutResult();
								mFirstTeamInControl = mMatch->getPenaltyShootout().firstTeamKicksNext();
								return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutPenaltykick));
							}
						}
					}
				}
			}
			else {
				mOutOfPlayClock.doCountdown(time);
				if(mOutOfPlayClock.check()) {
					if(mMatch->getBall()->grabbed())
						mMatch->getBall()->drop();
					mMatch->getBall()->setPosition(mRestartPosition);
					mMatch->getBall()->setVelocity(Vector3());
					mWaitForPenaltyShot.rewind();
				}
			}
			break;

		case MatchHalf::HalfTimePauseBegin:
		case MatchHalf::FullTimePauseBegin:
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
			if(!MatchHelpers::onOwnSideAndReady(*p)) {
				return false;
			}
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
		case MatchHalf::FullTimePauseBegin:
		case MatchHalf::FullTimePauseEnd:
		case MatchHalf::Finished:
			return true;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
		case MatchHalf::ExtraTimeFirstHalf:
		case MatchHalf::ExtraTimeSecondHalf:
			switch(mMatch->getPlayState()) {
				case PlayState::InPlay:
					return true;

				default:
					return MatchHelpers::playersPositionedForRestart(*p.getMatch(), p);
			}

		case MatchHalf::PenaltyShootout:
			switch(mMatch->getPlayState()) {
				case PlayState::InPlay:
					{
						bool isgoalie = p.isGoalkeeper();
						bool isdefending = p.getTeam()->isFirst() != mMatch->getPenaltyShootout().firstTeamKicksNext();
						bool alreadykicked = mWaitForPenaltyShot.running();
						return (isgoalie && isdefending && alreadykicked) || (!isdefending && !alreadykicked);
					}

				default:
					return MatchHelpers::playersPositionedForRestart(*p.getMatch(), p);
			}
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
				mRestartPosition.z = 0.0f;
				mFirstTeamInControl = !p.getTeam()->isFirst();
#ifdef DEBUG_CONTROLLING_TEAM
				std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - indirect free kick\n";
#endif
				mPlayerInControl = nullptr;
				std::cout << "Double touch by " << p.getName() << " - restart position: " << mRestartPosition << " by " << mFirstTeamInControl << "\n";
				mOutOfPlayClock.rewind();
				mMatch->setPlayState(PlayState::OutIndirectFreekick);
			}
			mRestartedPlayer = nullptr;
		}
	}
}

bool Referee::firstTeamAttacksUp() const
{
	return MatchHelpers::attacksUp(*mMatch->getTeam(0));
}

BallOutStatus Referee::getBallOutStatus() const
{
	RelVector3 bp(mMatch->convertAbsoluteToRelativeVector(mMatch->getBall()->getPosition()));
	RelVector3 br(mMatch->convertAbsoluteToRelativeVector(Vector3(BALL_RADIUS, BALL_RADIUS, BALL_RADIUS)));
	if(fabs(bp.v.x + br.v.x) > 1.0f) {
		return BallOutStatus::Throwin;
	}
	if(fabs(bp.v.y + br.v.y) > 1.0f) {
		if(fabs(mMatch->getBall()->getPosition().x) < GOAL_WIDTH_2 &&
				mMatch->getBall()->getPosition().z < GOAL_HEIGHT) {
			return BallOutStatus::Goal;
		}
		if((((bp.v.y < 0.0f) == mFirstTeamInControl) && firstTeamAttacksUp()) ||
		   (((bp.v.y < 0.0f) != mFirstTeamInControl) && !firstTeamAttacksUp())) {
			return BallOutStatus::CornerKick;
		}
		else {
			return BallOutStatus::GoalKick;
		}
	}
	return BallOutStatus::OnPitch;
}

void Referee::addPenaltyShootoutResult()
{
	BallOutStatus bst = getBallOutStatus();
	if(mMatch->getMatchHalf() == MatchHalf::PenaltyShootout) {
		switch(bst) {
			case BallOutStatus::Goal:
				mMatch->addPenaltyShootoutShot(true);
				std::cout << "Penalty shoot out goal!\n";
				break;

			case BallOutStatus::Throwin:
			case BallOutStatus::CornerKick:
			case BallOutStatus::GoalKick:
			case BallOutStatus::OnPitch:
				mMatch->addPenaltyShootoutShot(false);
				std::cout << "Penalty shoot out miss!\n";
				break;
		}
	}
}

boost::shared_ptr<RefereeAction> Referee::setOutOfPlay()
{
	BallOutStatus bst = getBallOutStatus();
	RelVector3 bp(mMatch->convertAbsoluteToRelativeVector(mMatch->getBall()->getPosition()));

	switch(bst) {
		case BallOutStatus::Throwin:
			mRestartPosition = mMatch->getBall()->getPosition();
			mRestartPosition.x = mMatch->getPitchWidth() * 0.5f;
			if(bp.v.x < 0.0f)
				mRestartPosition.x = -mRestartPosition.x;
			mRestartPosition.z = 0.0f;
			mFirstTeamInControl = !mFirstTeamInControl;
#ifdef DEBUG_CONTROLLING_TEAM
			std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - throwin\n";
#endif
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutThrowin));

		case BallOutStatus::Goal:
			mRestartPosition.x = 0.0f;
			mRestartPosition.y = 0.0f;
			mRestartPosition.z = 0.0f;
			bool firstscores;
			if(bp.v.y > 1.0f) {
				firstscores = firstTeamAttacksUp();
			}
			else {
				firstscores = !firstTeamAttacksUp();
			}
			mMatch->addGoal(firstscores);
			mFirstTeamInControl = !firstscores;
#ifdef DEBUG_CONTROLLING_TEAM
			std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - goal\n";
#endif
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutKickoff));

		case BallOutStatus::CornerKick:
			if(bp.v.x == 0.0f)
				bp.v.x = 1.0f;
			if(bp.v.y == 0.0f)
				bp.v.y = 1.0f;
			mRestartPosition.x = Common::signum(bp.v.x) * mMatch->getPitchWidth() * 0.5f;
			mRestartPosition.y = Common::signum(bp.v.y) * mMatch->getPitchHeight() * 0.5f;
			mRestartPosition.z = 0.0f;
			mFirstTeamInControl = !mFirstTeamInControl;
#ifdef DEBUG_CONTROLLING_TEAM
			std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - corner kick\n";
#endif
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutCornerkick));

		case BallOutStatus::GoalKick:
			mRestartPosition.x = 9.16f;
			if(bp.v.x < 0) {
				mRestartPosition.x = -mRestartPosition.x;
			}
			mRestartPosition.y = mMatch->getPitchHeight() * 0.5f - 5.5f;
			if(bp.v.y < 0) {
				mRestartPosition.y = -mRestartPosition.y;
			}
			mRestartPosition.z = 0.0f;
			mFirstTeamInControl = !mFirstTeamInControl;
#ifdef DEBUG_CONTROLLING_TEAM
			std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - goal kick\n";
#endif
			mPlayerInControl = nullptr;
			return boost::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutGoalkick));

		case BallOutStatus::OnPitch:
			break;
	}

	return boost::shared_ptr<RefereeAction>();
}

boost::shared_ptr<RefereeAction> Referee::setFoulRestart()
{
	assert(mFouledTeam != 0);
	mFirstTeamInControl = mFouledTeam == 2;
#ifdef DEBUG_CONTROLLING_TEAM
	std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - foul\n";
#endif
	mFouledTeam = 0;

	mRestartPosition = mFoulPosition;
	mRestartPosition.z = 0.0f;
	mPlayerInControl = nullptr;

	int pen = MatchHelpers::inPenaltyArea(*mMatch, mRestartPosition);
	if(pen) {
		if((MatchHelpers::attacksUp(*mMatch->getTeam(0)) == mFirstTeamInControl && pen ==  1) ||
		   (MatchHelpers::attacksUp(*mMatch->getTeam(0)) != mFirstTeamInControl && pen == -1)) {
			bool up = pen == 1;
			mRestartPosition.x = 0.0f;
			mRestartPosition.y = (up ? 1.0f : -1.0f) * (mMatch->getPitchHeight() * 0.5f - 11.00f);
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
	ballTouched(p);
}

void Referee::matchHalfChanged(MatchHalf m)
{
	mPlayerInControl = nullptr;
	if(m == MatchHalf::HalfTimePauseEnd || m == MatchHalf::ExtraTimeSecondHalf) {
		mFirstTeamInControl = false;
#ifdef DEBUG_CONTROLLING_TEAM
		std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - match half\n";
#endif
	} else if(m == MatchHalf::FullTimePauseEnd) {
		mFirstTeamInControl = true;
#ifdef DEBUG_CONTROLLING_TEAM
		std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - extra time\n";
#endif
	} else if(m == MatchHalf::PenaltyShootout) {
		mRestartPosition.x = 0.0f;
		mRestartPosition.y = 1.0f * (mMatch->getPitchHeight() * 0.5f - 11.00f);
		mMatch->setPlayState(PlayState::OutPenaltykick);
		mOutOfPlayClock.rewind();
	}
}

const Player* Referee::getPlayerInControl() const
{
	return mPlayerInControl;
}

void Referee::ballTouched(const Player& p)
{
	mFirstTeamInControl = p.getTeam()->isFirst();
#ifdef DEBUG_CONTROLLING_TEAM
	std::cout << __LINE__ << ": First team in control: " << mFirstTeamInControl << " - ball touched\n";
#endif
	mPlayerInControl = &p;
}

void Referee::playerTackled(const Player& tackled, const Player& tacklee)
{
	if((mFirstTeamInControl == tackled.getTeam()->isFirst()) ||
			MatchEntity::distanceBetween(tacklee, *mMatch->getBall()) > MAX_KICK_DISTANCE * 1.4f) {
		if(tackled.getTeam()->isFirst())
			mFouledTeam = 2;
		else
			mFouledTeam = 1;

		mFoulPosition = tackled.getPosition();
	}
}


