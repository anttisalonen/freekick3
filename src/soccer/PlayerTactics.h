#ifndef PLAYERTACTICS_H
#define PLAYERTACTICS_H

#include "match/Distance.h"

struct PlayerTactics {
	inline PlayerTactics(float pos, float radius);
	float WidthPosition;
	float Radius;
};

PlayerTactics::PlayerTactics(float pos, float radius)
	: WidthPosition(pos),
	Radius(radius)
{
}

#endif
