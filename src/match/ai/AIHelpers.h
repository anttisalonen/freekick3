#ifndef AIHELPERS_H
#define AIHELPERS_H

#include <boost/shared_ptr.hpp>
#include <functional>

#include "match/Player.h"
#include "match/PlayerActions.h"
#include "match/Distance.h"

class AIHelpers {
	public:
		static boost::shared_ptr<PlayerAction> createMoveActionTo(const Player& p,
				const Common::Vector3& pos, float threshold = 0.3f);
		static boost::shared_ptr<PlayerAction> createMoveActionToBall(const Player& p);
		static Common::Vector3 getShotPosition(const Player& p);
		static Common::Vector3 getPassPosition(const Player& p);
		static Common::Vector3 getPassKickVector(const Player& from, const Common::Vector3& to);
		static Common::Vector3 getPassKickVector(const Player& from, const Player& to);
		static Common::Vector3 getPassKickVector(const Player& from, const Common::Vector3& pos, const Common::Vector3& vel);
		static float checkTacticArea(const Player& p, float score, const Common::Vector3& pos);
		static float scaledDistanceFrom(float dist, float opt);
		static float scaledCoefficient(float dist, float maximum);
		static float checkKickSuccess(const Player& p, const Common::Vector3& v, float score);
		static float getPassForwardCoefficient(const Player& p, const Common::Vector3& v);
		static float getPassForwardCoefficient(const Player& p, const Player& tp);
		static float getDepthCoefficient(const Player& p);
		static float getDepthCoefficient(const Team& p, const Common::Vector3& v);
		static float getDepthCoefficient(const Player& p, const Common::Vector3& v);
		static bool opponentAttacking(const Player& p);

	private:
		static Common::Vector3 getPositionByFunc(const Player& p, std::function<float (const Common::Vector3& v)> func);
};

#endif

