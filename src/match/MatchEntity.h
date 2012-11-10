#ifndef MATCHENTITY_H
#define MATCHENTITY_H

#include "common/Vehicle.h"

#include "match/Distance.h"

class Match;

class MatchEntity : public Common::Vehicle {
	public:
		MatchEntity(Match* match, bool ball, const Common::Vector3& pos);
		virtual ~MatchEntity() { }
		const Match* getMatch() const;

	protected:
		Match* mMatch;
};

#endif

