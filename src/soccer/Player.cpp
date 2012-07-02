#include "soccer/Player.h"

namespace Soccer {

Player::Player(int id, const char* name,
		const PlayerSkills& skills)
	: mId(id),
	mName(name),
	mSkills(skills)
{
}

const PlayerSkills& Player::getSkills() const
{
	return mSkills;
}

const std::string& Player::getName() const
{
	return mName;
}

int Player::getId() const
{
	return mId;
}

Player::Player()
{
}

}
