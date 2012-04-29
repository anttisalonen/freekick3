#ifndef MATCHHELPERS_H
#define MATCHHELPERS_H

#include "Match.h"
#include "Distance.h"

class MatchHelpers {
	public:
		static double distanceToPitch(const Match& m,
				const AbsVector3& v);
		static bool allowedToKick(const Match& m,
				const Player& p);
		static bool nearestOwnPlayerTo(const Match& m,
				const Player& p, const AbsVector3& v);
		static AbsVector3 oppositeGoalPosition(const Player& p);
};

#endif

