#include <stdexcept>

#include "Referee.h"
#include "RefereeActions.h"

Referee::Referee()
	: mMatch(nullptr),
	mFirstTeamInControl(true),
	mOutOfPlayClock(1.0f)
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
					if(!onPitch(*mMatch->getBall())) {
						mOutOfPlayClock.rewind();
						mRestartPosition = mMatch->getBall()->getPosition();
						return setOutOfPlay();
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
					return true;
				default:
					if(p.getTeam()->isFirst() == mFirstTeamInControl) {
						mMatch->setPlayState(PlayState::InPlay);
						mFirstTeamInControl = p.getTeam()->isFirst();
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
	std::cout << "Ball position: " << mMatch->getBall()->getPosition().v << "\n";
	RelVector3 bp(mMatch->convertAbsoluteToRelativeVector(mMatch->getBall()->getPosition()));
	std::cout << "Relative ball position: " << bp.v << "\n";
	if(bp.v.x < -1.0f || bp.v.x > 1.0f) {
		return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutThrowin));
	}
	if(bp.v.y < -1.0f || bp.v.y > 1.0f) {
		if((((bp.v.y < -1.0f) != mFirstTeamInControl) && (mMatch->getMatchHalf() == MatchHalf::FirstHalf)) ||
		   (((bp.v.y < -1.0f) == mFirstTeamInControl) && (mMatch->getMatchHalf() == MatchHalf::SecondHalf))) {
			return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutCornerkick));
		}
		else {
			return std::shared_ptr<RefereeAction>(new ChangePlayStateRA(PlayState::OutGoalkick));
		}
	}
	throw std::runtime_error("Setting out of play even when the ball is in play");
}

