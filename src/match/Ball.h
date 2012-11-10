#ifndef BALL_H
#define BALL_H

#include "match/Distance.h"
#include "match/MatchEntity.h"

class Match;
class Player;

class Ball : public MatchEntity {
	public:
		Ball(Match* match);
		void update(float time) override;
		void kicked(Player* p);
		bool grabbed() const;
		void grab(Player* p);
		void drop();
		const Player* getGrabber() const;
		const Player* checkPlayerCollisions();
	private:
		bool checkCollision(const Player& p);
		Common::Vector3 mCollisionFreePoint;
		bool mGrabbed;
		Player* mGrabber;
};

#endif

