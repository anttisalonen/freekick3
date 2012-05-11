#include <stdexcept>
#include "match/ai/PlayerAIController.h"
#include "match/Match.h"
#include "match/MatchHelpers.h"
#include "match/Player.h"
#include "match/PlayerActions.h"
#include "match/ai/AIHelpers.h"

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
		case MatchHalf::HalfTimePauseEnd:
			if(mPlayer->getShirtNumber() >= 10 && MatchHelpers::myTeamInControl(*mPlayer))
				return AIHelpers::createMoveActionTo(*mPlayer, AbsVector3(mPlayer->getHomePosition().v.x < 0.0f ?
							-1.0f : 2.0f, 0, 0));
			else
				return AIHelpers::createMoveActionTo(*mPlayer,
						mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));

		case MatchHalf::HalfTimePauseBegin:
		case MatchHalf::Finished:
			return AIHelpers::createMoveActionTo(*mPlayer,
					mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getTeam()->getPausePosition()));

		case MatchHalf::FirstHalf:
		case MatchHalf::SecondHalf:
			switch(mPlayer->getMatch()->getPlayState()) {
				case PlayState::InPlay:
					return mPlayState->act(time);

				default:
					return actOffPlay(time);
			}
	}
	throw std::runtime_error("AI error: no state handler");
}

const std::string& PlayerAIController::getDescription() const
{
	return mPlayState->getDescription();
}

std::shared_ptr<PlayerAction> PlayerAIController::actOffPlay(double time)
{
	if(MatchHelpers::myTeamInControl(*mPlayer)) {
		bool nearest = MatchHelpers::nearestOwnPlayerTo(*mPlayer,
				mPlayer->getMatch()->getBall()->getPosition());
		bool shouldkickball =
			mPlayer->getMatch()->getPlayState() == PlayState::OutKickoff ?
			mPlayer->getShirtNumber() == 10 : nearest;
		if(shouldkickball) {
			float dist = (mPlayer->getMatch()->getBall()->getPosition().v -
					mPlayer->getPosition().v).length();
			bool cankickball = mPlayer->getMatch()->getPlayState() != PlayState::OutKickoff ||
				MatchHelpers::playersPositionedForKickoff(*mPlayer->getMatch(),
						*mPlayer);
			if(cankickball && dist < MAX_KICK_DISTANCE * 0.7f) {
				if(mKickInTimer.checkAndRewind()) {
					return mPlayState->act(time);
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
					if(!MatchHelpers::onOwnSide(*mPlayer)) {
						AbsVector3 v = mPlayer->getMatch()->getBall()->getPosition();
						if(MatchHelpers::attacksUp(*mPlayer))
							v.v.y -= 1.0f;
						else
							v.v.y += 1.0f;
						return AIHelpers::createMoveActionTo(*mPlayer, v);
					}
					else {
						return std::shared_ptr<PlayerAction>(new IdlePA());
					}
				}
			}
		}
		else {
			if(mPlayer->getMatch()->getPlayState() == PlayState::OutKickoff) {
				if(mPlayer->getShirtNumber() >= 10)
					return AIHelpers::createMoveActionTo(*mPlayer, AbsVector3(mPlayer->getHomePosition().v.x < 0.0f ?
								-1.0f : 2.0f, 0, 0));
				else
					return AIHelpers::createMoveActionTo(*mPlayer,
							mPlayer->getMatch()->convertRelativeToAbsoluteVector(mPlayer->getHomePosition()));
			}
			else {
				return std::shared_ptr<PlayerAction>(new IdlePA());
			}
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

void PlayerAIController::matchHalfChanged(MatchHalf m)
{
	mPlayState->matchHalfChanged(m);
}


