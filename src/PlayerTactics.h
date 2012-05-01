#ifndef PLAYERTACTICS_H
#define PLAYERTACTICS_H

struct PlayerTactics {
	inline PlayerTactics(bool offensive = false);
	bool mOffensive;
};

PlayerTactics::PlayerTactics(bool offensive)
	: mOffensive(offensive)
{
}

#endif
