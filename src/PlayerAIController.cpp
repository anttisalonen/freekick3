#include <stdexcept>
#include "PlayerAIController.h"
#include "Match.h"
#include "MatchHelpers.h"
#include "Player.h"
#include "PlayerActions.h"
#include "AIHelpers.h"

PlayerAIController::PlayerAIController(Player* p)
	: PlayerController(p),
	mKickInTimer(1.0f)
{
	mPlayState = std::shared_ptr<AIPlayController>(new AIPlayController(p));
}

std::shared_ptr<PlayerAction> PlayerAIController::act(double time)
{
	switch(mPlayer->getMatch()->getMatchHalf()) {
		case MatchHalf::NotStarted:
			return AIHelpers::createMoveActionTo(*mPlayer,
					mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));
		case MatchHalf::HalfTimePause:
		case MatchHalf::Finished:
			return AIHelpers::createMoveActionTo(*mPlayer,
					mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getTeam()->getPausePosition()));
		default:
			switch(mPlayer->getMatch()->getPlayState()) {
				case PlayState::InPlay:
					return mPlayState->act(time);

				default:
					if(MatchHelpers::allowedToKick(*mPlayer)) {
						bool nearest = MatchHelpers::nearestOwnPlayerTo(*mPlayer,
								mPlayer->getMatch()->getBall()->getPosition());
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
									return AIHelpers::createMoveActionTo(*mPlayer,
											mPlayer->getMatch()->getBall()->getPosition());
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
						if(mPlayer->getMatch()->getPlayState() == PlayState::OutKickoff) {
							return AIHelpers::createMoveActionTo(*mPlayer,
									mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));
						}
						else {
							return mPlayState->actOnRestart(time);
						}
					}
			}
	}
	throw std::runtime_error("AI error: no state handler");
}

const std::string& PlayerAIController::getDescription() const
{
	return mPlayState->getDescription();
}


