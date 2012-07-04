#include <algorithm>

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

std::string Player::getShorterName(const Player& p)
{
	std::string plname = p.getName();
	size_t spacepos = plname.find_first_of(' ');
	if(spacepos != plname.npos && spacepos < plname.size() + 1)
		plname.assign(plname, spacepos + 1, 30);
	return plname;
}

int Player::getSkillIndex(const Player& p)
{
	const PlayerSkills& sk = p.getSkills();
	int gkskill = sk.GoalKeeping * 1000;
	int fieldskill = (sk.Passing + sk.BallControl +
		sk.RunSpeed + sk.Heading +
		sk.ShotPower + sk.Tackling) * 1000.0f / 6;
	if(gkskill > fieldskill)
		return -std::min(1000, int(gkskill * 1.5f));
	else
		return std::min(1000, int(fieldskill * 1.5f));
}

}
