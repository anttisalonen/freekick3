#include <stdexcept>
#include "PlayerAIController.h"
#include "Match.h"
#include "MatchHelpers.h"
#include "Player.h"
#include "PlayerActions.h"

PlayerAIController::PlayerAIController(Player* p)
	: PlayerController(p),
	mKickInTimer(1.0f)
{
	if(p->isGoalkeeper())
		mCurrentPlayState = std::shared_ptr<PlayerController>(new AIGoalkeeperState(p));
	else
		mCurrentPlayState = std::shared_ptr<PlayerController>(new AIDefendState(p));
}

std::shared_ptr<PlayerAction> PlayerAIController::act(double time)
{
	switch(mPlayer->getMatch()->getMatchHalf()) {
		case MatchHalf::NotStarted:
			return createMoveActionTo(mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));
		case MatchHalf::HalfTimePause:
		case MatchHalf::Finished:
			return createMoveActionTo(mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getTeam()->getPausePosition()));
		default:
			switch(mPlayer->getMatch()->getPlayState()) {
				case PlayState::InPlay:
					return mCurrentPlayState->act(time);

				default:
					if(MatchHelpers::allowedToKick(*mPlayer->getMatch(), *mPlayer)) {
						bool nearest = MatchHelpers::nearestOwnPlayerTo(*mPlayer->getMatch(),
								*mPlayer, mPlayer->getMatch()->getBall()->getPosition());
						if(nearest) {
							float dist = (mPlayer->getMatch()->getBall()->getPosition().v -
									mPlayer->getPosition().v).length();
							if(dist < MAX_KICK_DISTANCE * 0.7f) {
								if(mKickInTimer.checkAndRewind()) {
									return std::shared_ptr<PlayerAction>(new
											KickBallPA(MatchHelpers::oppositeGoalPosition(*mPlayer),
												true));
								}
								else {
									mKickInTimer.doCountdown(time);
									return std::shared_ptr<PlayerAction>(new IdlePA());
								}
							}
							else {
								if(MatchHelpers::distanceToPitch(*mPlayer->getMatch(),
											mPlayer->getMatch()->getBall()->getPosition())
										< 1.0f) {
									return createMoveActionTo(mPlayer->getMatch()->getBall()->getPosition());
								} else {
									return std::shared_ptr<PlayerAction>(new IdlePA());
								}
							}
						}
						else {
							return std::shared_ptr<PlayerAction>(new IdlePA());
						}
					}
					else {
						/* TODO: move to a good position */
						return std::shared_ptr<PlayerAction>(new IdlePA());
					}
			}
	}
	throw std::runtime_error("AI error: no state handler");
}

std::shared_ptr<PlayerAction> PlayerAIController::createMoveActionTo(const AbsVector3& pos) const
{
	AbsVector3 v(pos);
	v.v -= mPlayer->getPosition().v;
	if(v.v.length() < 0.3f) {
		return std::shared_ptr<PlayerAction>(new IdlePA());
	}
	else {
		return std::shared_ptr<PlayerAction>(new RunToPA(v));
	}
}

