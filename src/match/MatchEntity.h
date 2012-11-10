#ifndef MATCHENTITY_H
#define MATCHENTITY_H

#include "common/Entity.h"

#include "match/Distance.h"

class Match;

class MatchEntity : public Common::Entity {
	public:
		MatchEntity(Match* match, const Common::Vector3& pos);
		virtual ~MatchEntity() { }
		const Match* getMatch() const;

	protected:
		Match* mMatch;
};

#endif

