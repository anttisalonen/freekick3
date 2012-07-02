#ifndef PLAYERTACTICS_H
#define PLAYERTACTICS_H

#include "common/Serialization.h"

#include "match/Distance.h"

namespace Soccer {

enum class PlayerPosition {
	Goalkeeper,
	Defender,
	Midfielder,
	Forward
};

struct PlayerTactics {
	inline PlayerTactics(float pos, float radius, PlayerPosition ppos,
			bool offensive);
	inline PlayerTactics(); // serialization
	float WidthPosition;
	float Radius;
	PlayerPosition Position;
	bool Offensive;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & WidthPosition;
		ar & Radius;
		ar & Position;
		ar & Offensive;
	}
};

PlayerTactics::PlayerTactics(float pos, float radius, PlayerPosition ppos,
		bool offensive)
	: WidthPosition(pos),
	Radius(radius),
	Position(ppos),
	Offensive(offensive)
{
}

PlayerTactics::PlayerTactics()
{
}

}

#endif
