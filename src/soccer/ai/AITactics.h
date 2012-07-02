#ifndef SOCCER_AI_AITACTICS_H
#define SOCCER_AI_AITACTICS_H

namespace Soccer {

class TeamTactics;
class Team;

struct PlayerSkillValue {
	float Goalkeeping;
	float Defending;
	float Midfield;
	float Forward;
};

class AITactics {
	public:
		static TeamTactics createTeamTactics(const Team& team);
		static PlayerSkillValue calculatePlayerSkill(const Player& p);

};

}

#endif
