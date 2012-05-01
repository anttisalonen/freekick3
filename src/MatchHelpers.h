#ifndef MATCHHELPERS_H
#define MATCHHELPERS_H

#include "Match.h"
#include "Distance.h"

#define MAX_KICK_DISTANCE 1.0

class MatchHelpers {
	public:
		static double distanceToPitch(const Match& m,
				const AbsVector3& v);
		static bool allowedToKick(const Player& p);
		static AbsVector3 oppositeGoalPosition(const Player& p);
		static AbsVector3 oppositePenaltySpotPosition(const Player& p);
		static bool canKickBall(const Player& p);
		static Player* nearestOwnPlayerToPlayer(const Team& t, const Player& p);
		static Player* nearestOwnPlayerToBall(const Team& t);
		static Player* nearestOwnPlayerTo(const Team& t, const AbsVector3& v);
		static bool nearestOwnPlayerTo(const Player& p, const AbsVector3& v);
		static bool myTeamInControl(const Player& p);
		static const std::vector<std::shared_ptr<Player>>& getOpposingPlayers(const Player& p);
		static const std::vector<std::shared_ptr<Player>>& getOwnPlayers(const Player& p);
		static const std::vector<std::shared_ptr<Player>>& getTeamPlayers(const Match& m, unsigned int idx);
		static bool attacksUp(const Player& p);
};

#endif

