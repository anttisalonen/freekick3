#ifndef PLAYERTACTICS_H
#define PLAYERTACTICS_H

#include "common/Serialization.h"

#include "match/Distance.h"

struct PlayerTactics {
	inline PlayerTactics(float pos, float radius);
	inline PlayerTactics(); // serialization
	float WidthPosition;
	float Radius;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & WidthPosition;
		ar & Radius;
	}
};

PlayerTactics::PlayerTactics(float pos, float radius)
	: WidthPosition(pos),
	Radius(radius)
{
}

PlayerTactics::PlayerTactics()
{
}

#endif
