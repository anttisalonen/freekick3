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
		void setPosition(const AbsVector3& v);
		const AbsVector3& getPosition() const;
		const AbsVector3& getVelocity() const;
		virtual void update(float time);
		static inline AbsVector3 vectorFromTo(const MatchEntity& me1,
				const MatchEntity& me2);
		static inline double distanceBetween(const MatchEntity& me1,
				const MatchEntity& me2);
	protected:
		Match* mMatch;
		AbsVector3 mPosition;
		AbsVector3 mVelocity;
};

AbsVector3 MatchEntity::vectorFromTo(const MatchEntity& me1,
		const MatchEntity& me2)
{
	return AbsVector3(me2.getPosition().v - me1.getPosition().v);
}

double MatchEntity::distanceBetween(const MatchEntity& me1,
		const MatchEntity& me2)
{
	return vectorFromTo(me1, me2).v.length();
}

#endif

