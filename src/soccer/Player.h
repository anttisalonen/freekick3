#ifndef SOCCER_PLAYER_H
#define SOCCER_PLAYER_H

namespace Soccer {

struct PlayerSkills {
	PlayerSkills()
		: KickPower(1.0f),
		RunSpeed(1.0f),
		BallControl(1.0f) { }
	float KickPower;
	float RunSpeed;
	float BallControl;
};

typedef int ShirtNumber;

class Player {
	public:
		Player(ShirtNumber sn, bool gk,
				const PlayerSkills& skills);
		virtual ~Player() { }
		bool isGoalkeeper() const;
		int getShirtNumber() const;
		const PlayerSkills& getSkills() const;
	protected:
		bool mGoalkeeper;
		ShirtNumber mShirtNumber;
		PlayerSkills mSkills;
};

}

#endif

