#ifndef SOCCER_PLAYER_H
#define SOCCER_PLAYER_H

#include <string>

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
};

enum class PlayerPosition {
	Goalkeeper,
	Defender,
	Midfielder,
	Forward
};

class Player {
	public:
		Player(int id, const char* name, PlayerPosition pos,
				const PlayerSkills& skills);
		virtual ~Player() { }
		bool isGoalkeeper() const;
		const PlayerSkills& getSkills() const;
		PlayerPosition getPlayerPosition() const;
		const std::string& getName() const;
		int getId() const;
	protected:
		int mId;
		std::string mName;
		PlayerPosition mPlayerPosition;
		PlayerSkills mSkills;
};

}

#endif

