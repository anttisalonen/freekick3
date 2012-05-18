#ifndef TEAM_H
#define TEAM_H

#include <vector>

#include "soccer/Player.h"
#include "soccer/Team.h"

#include "match/Player.h"
#include "match/Distance.h"

enum class MatchHalf;

class Team : public Soccer::Team {
	public:
		Team(Match* match, const Soccer::Team& t, bool first);
		void addPlayer(const Soccer::Player& pl);
		Player* getPlayer(unsigned int idx);
		const Player* getPlayer(unsigned int idx) const;
		unsigned int getNumPlayers() const;
		const std::vector<std::shared_ptr<Player>>& getPlayers() const;
		RelVector3 getPausePosition() const;
		bool isFirst() const;
		void act(double time);
		const Match* getMatch() const;
		Player* getPlayerNearestToBall() const;
		float getSupportingPositionScoreAt(const AbsVector3& pos) const;
		void matchHalfChanged(MatchHalf m);
		void setPlayerReceivingPass(Player* p);
		Player* getPlayerReceivingPass();
		void ballKicked(Player* p);
	private:
		void updatePlayerNearestToBall();
		void updateSupportingPositions();
		float calculateSupportingPositionScoreAt(const AbsVector3& pos) const;
		Match* mMatch;
		bool mFirst;
		std::vector<std::shared_ptr<Player>> mPlayers;
		Player* mPlayerNearestToBall;
		Countdown mSupportingPositionsTimer;
		std::vector<std::vector<float>> mSupportingPositions;
		Player* mPlayerReceivingPass;
};

#endif
