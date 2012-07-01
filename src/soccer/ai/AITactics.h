#ifndef SOCCER_AI_AITACTICS_H
#define SOCCER_AI_AITACTICS_H

namespace Soccer {

class TeamTactics;
class Team;

class AITactics {
	public:
		static TeamTactics createTeamTactics(const Team& team);

};

}

#endif
