#include "Referee.h"
#include "RefereeActions.h"

Referee::Referee()
	: mMatch(nullptr)
{
}

void Referee::setMatch(Match* m)
{
	mMatch = m;
}

std::shared_ptr<RefereeAction> Referee::act()
{
	switch(mMatch->getMatchHalf()) {
		case MatchHalf::NotStarted:
		case MatchHalf::HalfTimePause:
			if(allPlayersOnOwnSideAndReady())
				return std::shared_ptr<RefereeAction>(new ChangeMatchHalfRA(MatchHalf::FirstHalf));
			else
				break;
		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			/* TODO */
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
					/* TODO: record controlling team */
					mMatch->setPlayState(PlayState::InPlay);
					return true;
			}
		case MatchHalf::Finished:
			return true;
	}
	return false;
}

