#include "soccer/Player.h"

namespace Soccer {

Player::Player(int id, const char* name, PlayerPosition pos,
		const PlayerSkills& skills)
	: mId(id),
	mName(name),
	mPosition(pos),
	mSkills(skills)
{
}

bool Player::isGoalkeeper() const
{
	return mPosition == PlayerPosition::Goalkeeper;
}

const PlayerSkills& Player::getSkills() const
{
	return mSkills;
}

PlayerPosition Player::getPlayerPosition() const
{
	return mPosition;
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
