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
		static TeamTactics createTeamTactics(const Team& team, unsigned int def = 0,
				unsigned int mid = 0, unsigned int forw = 0);
		static TeamTactics updateTeamTactics(const TeamTactics& t, unsigned int def,
				unsigned int mid, unsigned int forw);
		static PlayerSkillValue calculatePlayerSkill(const Player& p);

		static PlayerTactics createPlayerTactics(Soccer::PlayerPosition pos,
				unsigned int num, unsigned int howmany, bool offensive);
};

}

#endif
