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
	/* field skill index is the average of the top three skills.
	 * goal keeper skill index is just the goal keeping skill,
	 * scaled down a bit for compensation. */
	int gkskill = sk.GoalKeeping * 950;
	auto s = getTopSkillsString(p);
	float fieldskill = 0.0f;
	assert(s.size() == 3);
	for(auto c : s) {
		switch(c) {
			default:
			case 'P': fieldskill += sk.Passing; break; 
			case 'S': fieldskill += sk.RunSpeed; break; 
			case 'C': fieldskill += sk.BallControl; break; 
			case 'H': fieldskill += sk.Heading; break; 
			case 'V': fieldskill += sk.ShotPower; break; 
			case 'T': fieldskill += sk.Tackling; break; 
		}
	}
	fieldskill *= 1000.0f;
	fieldskill /= float(s.size());
	if(gkskill > fieldskill)
		return -std::min(1000, int(gkskill * 1.0f));
	else
		return std::min(1000, int(fieldskill * 1.0f));
}

std::string Player::getTopSkillsString(const Player& p)
{
	std::string ret;

	std::map<float, char> skills;

	const PlayerSkills& sk = p.getSkills();
	skills.insert(std::make_pair(sk.Passing, 'P'));
	skills.insert(std::make_pair(sk.RunSpeed, 'S'));
	skills.insert(std::make_pair(sk.BallControl, 'C'));
	skills.insert(std::make_pair(sk.Heading, 'H'));
	skills.insert(std::make_pair(sk.ShotPower, 'V'));
	skills.insert(std::make_pair(sk.Tackling, 'T'));

	int i = 0;
	for(std::map<float, char>::reverse_iterator it = skills.rbegin(); i < 3; i++, it++) {
		ret += it->second;
	}
	return ret;
}

}
