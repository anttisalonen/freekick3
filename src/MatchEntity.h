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
		void setVelocity(const AbsVector3& v);
		const AbsVector3& getPosition() const;
		const AbsVector3& getVelocity() const;
		void update(float time);
	private:
		Match* mMatch;
		AbsVector3 mPosition;
		AbsVector3 mVelocity;
};

#endif

