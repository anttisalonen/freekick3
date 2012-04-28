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
	/* TODO */
	return std::shared_ptr<RefereeAction>(new IdleRA());
}

