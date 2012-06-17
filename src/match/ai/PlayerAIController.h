#ifndef PLAYERAICONTROLLER_H
#define PLAYERAICONTROLLER_H

#include "match/PlayerController.h"
#include "match/ai/AIPlayStates.h"
#include "match/Distance.h"
#include "match/Clock.h"

class PlayerAIController : public PlayerController {
	public:
		PlayerAIController(Player* p);
		boost::shared_ptr<PlayerAction> act(double time);
		const std::string& getDescription() const;
		virtual void matchHalfChanged(MatchHalf m) override;
	protected:
		boost::shared_ptr<PlayerAction> createMoveActionTo(const AbsVector3& pos) const;
	private:
		boost::shared_ptr<PlayerAction> actOffPlay(double time);
		boost::shared_ptr<PlayerAction> doRestart(double time);
		boost::shared_ptr<PlayerAction> gotoKickPositionOrKick(double time, const AbsVector3& pos);
		Countdown mKickInTimer;
		boost::shared_ptr<AIPlayController> mPlayState;
};

#endif

