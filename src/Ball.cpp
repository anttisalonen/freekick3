#include "Ball.h"
#include "Match.h"

Ball::Ball(Match* match)
	: MatchEntity(match, Vector3(0, 0, 0))
{
}

