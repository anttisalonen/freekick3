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
	/* TODO */
	return false;
}

