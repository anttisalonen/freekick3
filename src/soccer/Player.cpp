#include "soccer/Player.h"

namespace Soccer {

Player::Player(ShirtNumber sn, bool gk,
		const PlayerSkills& skills)
	: mGoalkeeper(gk),
	mShirtNumber(sn),
	mSkills(skills)
{
}

bool Player::isGoalkeeper() const
{
	return mGoalkeeper;
}

int Player::getShirtNumber() const
{
	return mShirtNumber;
}

const PlayerSkills& Player::getSkills() const
{
	return mSkills;
}

}
