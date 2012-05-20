#include <stdlib.h>

#include <stdexcept>

#include "match/Math.h"
#include "match/MatchHelpers.h"
#include "match/Referee.h"
#include "match/RefereeActions.h"

Referee::Referee()
	: mMatch(nullptr),
	mFirstTeamInControl(true),
	mOutOfPlayClock(1.0f),
	mWaitForResumeClock(0.1f)
{
}

void Referee::setMatch(Match* m)
{
	mMatch = m;
}

std::shared_ptr<RefereeAction> Referee::act(double time)
{
	switch(mMatch->getMatchHalf()) {
		case MatchHalf::NotStarted:
		case MatchHalf::HalfTimePauseEnd:
			if(allPlayersOnOwnSideAndReady()) {
				mFirstTeamInControl = mMatch->getMatchHalf() == MatchHalf::NotStarted;
				if(mFirstTeamInControl)
					return std::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::FirstHalf));
				else
					return std::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::SecondHalf));
			}
			break;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			if(!mOutOfPlayClock.running()) {
				if(mMatch->getPlayState() == PlayState::InPlay) {
					if(!mWaitForResumeClock.running()) {
						if(!onPitch(*mMatch->getBall())) {
							std::shared_ptr<RefereeAction> act = setOutOfPlay();
							if(act) {
								mOutOfPlayClock.rewind();
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
	return std::shared_ptr<RefereeAction>(new IdleRA());
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

bool Referee::onPitch(const MatchEntity& m) const
{
	return MatchHelpers::onPitch(*mMatch, m.getPosition());
}

bool Referee::ballKicked(const Player& p, const AbsVector3& vel)
{
	switch(mMatch->getMatchHalf()) {
		case MatchHalf::NotStarted:
		case MatchHalf::HalfTimePauseBegin:
		case MatchHalf::HalfTimePauseEnd:
			return false;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			switch(mMatch->getPlayState()) {
				case PlayState::InPlay:
					mFirstTeamInControl = p.getTeam()->isFirst();
					return true;
				default:
					if(p.getTeam()->isFirst() == mFirstTeamInControl) {
						mMatch->setPlayState(PlayState::InPlay);
						mFirstTeamInControl = p.getTeam()->isFirst();
						mWaitForResumeClock.rewind();
						return true;
					}
					else {
						return false;
					}
			}

		case MatchHalf::Finished:
			return true;
	}
	return false;
}

std::shared_ptr<RefereeAction> Referee::setOutOfPlay()
{
	mFirstTeamInControl = !mFirstTeamInControl;
	RelVector3 bp(mMatch->convertAbsoluteToRelativeVector(mMatch->getBall()->getPosition()));
	RelVector3 br(mMatch->convertAbsoluteToRelativeVector(AbsVector3(BALL_RADIUS, BALL_RADIUS, BALL_RADIUS)));
	if(fabs(bp.v.x + br.v.x) > 1.0f) {
		mRestartPosition = mMatch->getBall()->getPosition();
		mRestartPosition.v.x = mMatch->getPitchWidth() * 0.5f;
		if(bp.v.x < 0.0f)
			mRestartPosition.v.x = -mRestartPosition.v.x;
		return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutThrowin));
	}
	if(fabs(bp.v.y + br.v.y) > 1.0f) {
		if(fabs(mMatch->getBall()->getPosition().v.x) < GOAL_WIDTH_2 &&
				mMatch->getBall()->getPosition().v.z < GOAL_HEIGHT) {
			// goal
			mRestartPosition.v.x = 0.0f;
			mRestartPosition.v.y = 0.0f;
			bool firstscores;
			if(bp.v.y > 1.0f) {
				firstscores = mMatch->getMatchHalf() == MatchHalf::FirstHalf;
			}
			else {
				firstscores = mMatch->getMatchHalf() != MatchHalf::FirstHalf;
			}
			mMatch->addGoal(firstscores);
			mFirstTeamInControl = !firstscores;
			return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutKickoff));
		}
		if((((bp.v.y < -1.0f) != mFirstTeamInControl) && (mMatch->getMatchHalf() == MatchHalf::FirstHalf)) ||
		   (((bp.v.y < -1.0f) == mFirstTeamInControl) && (mMatch->getMatchHalf() == MatchHalf::SecondHalf))) {
			if(bp.v.x == 0.0f)
				bp.v.x = 1.0f;
			if(bp.v.y == 0.0f)
				bp.v.y = 1.0f;
			mRestartPosition.v.x = signum(bp.v.x) * mMatch->getPitchWidth() * 0.5f;
			mRestartPosition.v.y = signum(bp.v.y) * mMatch->getPitchHeight() * 0.5f;
			return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutCornerkick));
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
			return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutGoalkick));
		}
	}
	return nullptr;
}

bool Referee::isFirstTeamInControl() const
{
	return mFirstTeamInControl;
}

bool Referee::ballGrabbed(const Player& p)
{
	bool in_x = fabs(p.getPosition().v.x) < 20.15f;
	bool in_y;
	float yp = p.getPosition().v.y;

	if(MatchHelpers::attacksUp(p)) {
		in_y = yp < p.getMatch()->getPitchHeight() * -0.5f + 16.5f;
	}
	else {
		in_y = yp > p.getMatch()->getPitchHeight() * 0.5f - 16.5f;
	}
	in_y &= onPitch(p);

	if(!in_y || !in_x)
		return false;

	mFirstTeamInControl = p.getTeam()->isFirst();
	return true;
}

void Referee::matchHalfChanged(MatchHalf m)
{
	if(m == MatchHalf::HalfTimePauseEnd) {
		mFirstTeamInControl = false;
	}
}


