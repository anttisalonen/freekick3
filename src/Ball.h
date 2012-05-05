#ifndef BALL_H
#define BALL_H

#include "Distance.h"
#include "MatchEntity.h"

class Match;
class Player;

class Ball : public MatchEntity {
	public:
		Ball(Match* match);
		void update(float time) override;
		void kicked();
	private:
		void checkCollision(const Player& p);
		AbsVector3 mCollisionFreePoint;
};

#endif

