#include "soccer/Player.h"

namespace Soccer {

Player::Player(int id, const char* name, PlayerPosition pos,
		const PlayerSkills& skills)
	: mId(id),
	mName(name),
	mPlayerPosition(pos),
	mSkills(skills)
{
}

bool Player::isGoalkeeper() const
{
	return mPlayerPosition == PlayerPosition::Goalkeeper;
}

const PlayerSkills& Player::getSkills() const
{
	return mSkills;
}

PlayerPosition Player::getPlayerPosition() const
{
	return mPlayerPosition;
}

const std::string& Player::getName() const
{
	return mName;
}

int Player::getId() const
{
	return mId;
}

}
