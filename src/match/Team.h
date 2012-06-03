#ifndef TEAM_H
#define TEAM_H

#include <vector>

#include "soccer/Player.h"
#include "soccer/Team.h"

#include "match/Player.h"
#include "match/Distance.h"

enum class MatchHalf;

class Team : public Soccer::StatefulTeam {
	struct OffensivePosition {
		float ShotScore;
		float PassScore;
		OffensivePosition()
			: ShotScore(0.0f),
			PassScore(0.0f) { }
	};

	public:
		Team(Match* match, const Soccer::StatefulTeam& t, bool first);
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
		float getShotScoreAt(const AbsVector3& pos) const;
		float getPassScoreAt(const AbsVector3& pos) const;
		void matchHalfChanged(MatchHalf m);
		void setPlayerReceivingPass(Player* p);
		Player* getPlayerReceivingPass();
		void ballKicked(Player* p);
		bool isOffsidePosition(const AbsVector3& pos) const;
	private:
		void updatePlayerNearestToBall();
		void updateSupportingPositions();
		float calculateShotScoreAt(const AbsVector3& pos) const;
		float calculatePassScoreAt(const std::vector<std::shared_ptr<Player>>& offensivePlayers,
				const AbsVector3& pos) const;
		void getSupportPositionCoordinates(const AbsVector3& pos, unsigned int& i, unsigned int& j) const;
		Match* mMatch;
		bool mFirst;
		std::vector<std::shared_ptr<Player>> mPlayers;
		Player* mPlayerNearestToBall;
		Countdown mSupportingPositionsTimer;
		std::vector<std::vector<OffensivePosition>> mSupportingPositions;
		Player* mPlayerReceivingPass;
};

#endif
