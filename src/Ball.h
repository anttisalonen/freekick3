#ifndef BALL_H
#define BALL_H

#include "MatchEntity.h"

class Match;

class Ball : public MatchEntity {
	public:
		Ball(Match* match);
		void update(float time) override;
};

#endif

