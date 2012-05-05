#include <stdlib.h>

#include <stdexcept>

#include "Math.h"
#include "MatchHelpers.h"
#include "Referee.h"
#include "RefereeActions.h"

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
		case MatchHalf::HalfTimePause:
			if(allPlayersOnOwnSideAndReady()) {
				mFirstTeamInControl = mMatch->getMatchHalf() == MatchHalf::NotStarted;
				return std::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::FirstHalf));
			}
			break;

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			if(!mOutOfPlayClock.running()) {
				if(mMatch->getPlayState() == PlayState::InPlay) {
					if(!mWaitForResumeClock.running()) {
						if(!onPitch(*mMatch->getBall())) {
							mOutOfPlayClock.rewind();
							return setOutOfPlay();
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

		case MatchHalf::Finished:
			break;
	}
	return std::shared_ptr<RefereeAction>(new IdleRA());
}

bool Referee::allPlayersOnOwnSideAndReady() const
{
	for(int i = 0; i < 2; i++) {
		for(auto& p : mMatch->getTeam(i)->getPlayers()) {
			if(mMatch->getTeam(i)->isFirst() != (p->getPosition().v.y <= 0.0f)) {
				return false;
			}
			if(p->getVelocity().v.length() > 1.0f) {
				return false;
			}
			if(!onPitch(*p)) {
				return false;
			}
		}
	}
	return true;
}

bool Referee::onPitch(const MatchEntity& m) const
{
	const AbsVector3& v = m.getPosition();
	float pw2 = mMatch->getPitchWidth() / 2.0f;
	float ph2 = mMatch->getPitchHeight() / 2.0f;
	return v.v.x >= -pw2 && v.v.y >= -ph2 && v.v.x <= pw2 && v.v.y <= ph2;
}

bool Referee::ballKicked(const Player& p, const AbsVector3& vel)
{
	switch(mMatch->getMatchHalf()) {
		case MatchHalf::NotStarted:
		case MatchHalf::HalfTimePause:
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
	if(bp.v.x < -1.0f || bp.v.x > 1.0f) {
		mRestartPosition = mMatch->getBall()->getPosition();
		return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutThrowin));
	}
	if(bp.v.y < -1.0f || bp.v.y > 1.0f) {
		if(abs(mMatch->getBall()->getPosition().v.x) < 3.66f) {
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
	throw std::runtime_error("Setting out of play even when the ball is in play");
}

bool Referee::isFirstTeamInControl() const
{
	return mFirstTeamInControl;
}

bool Referee::kickSiteClear() const
{
	if(!playing(mMatch->getMatchHalf()))
		return true;

	switch(mMatch->getPlayState()) {
		case PlayState::InPlay:
			return true;
		case PlayState::OutKickoff:
			return allPlayersOnOwnSideAndReady();
		default:
			/* TODO: add restarts here when AI is able to clear site */
			return true;
	}
	return true;
}

bool Referee::ballGrabbed(const Player& p)
{
	bool in_x = abs(p.getPosition().v.x) < 20.15f;
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


