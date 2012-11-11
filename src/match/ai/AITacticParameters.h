#ifndef MATCH_AI_AITACTICPARAMETERS_H
#define MATCH_AI_AITACTICPARAMETERS_H

#include "soccer/Team.h"

struct AITacticParameters {
	AITacticParameters(const Soccer::StatefulTeam& tt);
	float PassActionCoefficient;
	float PassRiskCoefficient;
	float LongPassActionCoefficient;
	float DribbleActionCoefficient;
	float ShootActionCoefficient;
	float ShootCloseCoefficient;
	float ClearActionCoefficient;
	float FetchBallActionCoefficient;
	float GuardActionCoefficient;
	float BlockActionCoefficient;
	float BlockPassActionCoefficient;
	float GuardAreaActionCoefficient;
	float TackleActionCoefficient;
};



#endif
