#ifndef SOCCER_PLAYER_H
#define SOCCER_PLAYER_H

#include <string>

#include "common/Serialization.h"

namespace Soccer {

struct PlayerSkills {
	PlayerSkills()
		: ShotPower(1.0f),
		Passing(1.0f),
		RunSpeed(1.0f),
		BallControl(1.0f),
		Tackling(1.0f),
		Heading(1.0f),
		GoalKeeping(1.0f)
		{ }
	float ShotPower;
	float Passing;
	float RunSpeed;
	float BallControl;
	float Tackling;
	float Heading;
	float GoalKeeping;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & ShotPower;
		ar & Passing;
		ar & RunSpeed;
		ar & BallControl;
		ar & Tackling;
		ar & Heading;
		ar & GoalKeeping;
	}
};

class Player {
	public:
		Player(int id, const char* name,
				const PlayerSkills& skills);
		virtual ~Player() { }
		bool isGoalkeeper() const;
		const PlayerSkills& getSkills() const;
		const std::string& getName() const;
		int getId() const;

		static std::string getShorterName(const Player& p);
		static int getSkillIndex(const Player& p);
	protected:
		int mId;
		std::string mName;
		PlayerSkills mSkills;

	private:
		Player(); // serialization
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mId;
			ar & mName;
			ar & mSkills;
		}
};

}

#endif

