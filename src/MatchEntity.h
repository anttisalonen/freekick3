#ifndef MATCHENTITY_H
#define MATCHENTITY_H

#include "Distance.h"

class Match;

class MatchEntity {
	public:
		MatchEntity(Match* match, const AbsVector3& pos);
		virtual ~MatchEntity() { }
		const Match* getMatch() const;
		void move(const AbsVector3& v);
		const AbsVector3& getPosition() const;
	private:
		Match* mMatch;
		AbsVector3 mPosition;
};

#endif

