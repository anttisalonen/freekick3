#include <assert.h>

#include "common/Math.h"

#include "match/Team.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

#define SUPPORTING_POS_RESOLUTION 4

Team::Team(Match* match, const Soccer::StatefulTeam& t, bool first)
	: Soccer::StatefulTeam(t),
	mMatch(match),
	mFirst(first),
	mPlayerNearestToBall(nullptr),
	mSupportingPositionsTimer(0.5f),
	mPlayerReceivingPass(nullptr),
	mAITacticParameters(new AITacticParameters(t))
{
	for(unsigned int j = SUPPORTING_POS_RESOLUTION * 2;
			j <= match->getPitchHeight() - SUPPORTING_POS_RESOLUTION;
			j += SUPPORTING_POS_RESOLUTION) {
		mSupportingPositions.push_back(std::vector<OffensivePosition>());
		for(unsigned int i = SUPPORTING_POS_RESOLUTION * 2;
				i <= match->getPitchWidth() - SUPPORTING_POS_RESOLUTION;
				i += SUPPORTING_POS_RESOLUTION * 2) {
			mSupportingPositions[mSupportingPositions.size() - 1].push_back(OffensivePosition());
		}
	}
}

void Team::addPlayer(const Soccer::Player& pl)
{
	const auto it = mTactics.mTactics.find(pl.getId());
	assert(it != mTactics.mTactics.end());
	boost::shared_ptr<Player> p(new Player(mMatch, this, pl, mPlayers.size() + 1, it->second));
	mPlayers.push_back(p);
}

Player* Team::getPlayer(unsigned int idx)
{
	if(idx >= mPlayers.size())
		return nullptr;
	else
		return mPlayers[idx].get();
}

const Player* Team::getPlayer(unsigned int idx) const
{
	if(idx >= mPlayers.size())
		return nullptr;
	else
		return mPlayers[idx].get();
}

const Match* Team::getMatch() const
{
	return mMatch;
}

unsigned int Team::getNumPlayers() const
{
	return mPlayers.size();
}

const std::vector<boost::shared_ptr<Player>>& Team::getPlayers() const
{
	return mPlayers;
}

RelVector3 Team::getPausePosition() const
{
	return RelVector3(1.2f, 0.0f, 0.0f);
}

bool Team::isFirst() const
{
	return mFirst;
}

void Team::act(double time)
{
	updatePlayerNearestToBall();
	mSupportingPositionsTimer.doCountdown(time);
	if(mSupportingPositionsTimer.checkAndRewind()) {
		updateSupportingPositions();
	}
}

Player* Team::getPlayerNearestToBall() const
{
	return mPlayerNearestToBall;
}

void Team::updatePlayerNearestToBall()
{
	if(mPlayers.size())
		mPlayerNearestToBall = MatchHelpers::nearestOwnFieldPlayerToBall(*this);
}

void Team::getSupportPositionCoordinates(const AbsVector3& pos, unsigned int& i, unsigned int& j) const
{
	i = std::max(0, (int)(pos.v.x + mMatch->getPitchWidth() * 0.5f) / SUPPORTING_POS_RESOLUTION - 2);
	j = std::max(0, (int)(pos.v.y + mMatch->getPitchHeight() * 0.5f) / SUPPORTING_POS_RESOLUTION - 2);
	if(j >= mSupportingPositions.size())
		j = mSupportingPositions.size() - 1;
	if(i >= mSupportingPositions[j].size())
		i = mSupportingPositions[j].size() - 1;
	// printf("Support coords: (%3.1f, %3.1f) => (%d, %d) = %3.3f\n", pos.v.x, pos.v.y, i, j, mSupportingPositions[j][i]);
}

float Team::getShotScoreAt(const AbsVector3& pos) const
{
	unsigned int i, j;
	getSupportPositionCoordinates(pos, i, j);
	return mSupportingPositions.at(j).at(i).ShotScore;
}

float Team::getPassScoreAt(const AbsVector3& pos) const
{
	unsigned int i, j;
	getSupportPositionCoordinates(pos, i, j);
	return mSupportingPositions.at(j).at(i).PassScore;
}

void Team::updateSupportingPositions()
{
	std::vector<boost::shared_ptr<Player>> offensivePlayers;
	Vector3 oppgoal = MatchHelpers::oppositeGoalPosition(*this).v;
	for(auto pl : mPlayers) {
		float len = (pl->getPosition().v - oppgoal).length();
		if((len < 50.0f && pl->getPlayerPosition() == Soccer::PlayerPosition::Forward) ||
			len < 30.0f) {
			offensivePlayers.push_back(pl);
		}
	}

	for(unsigned int j = 0; j < mSupportingPositions.size(); j++) {
		float y = -mMatch->getPitchHeight() * 0.5f + SUPPORTING_POS_RESOLUTION * (j + 2);

		bool offside = isOffsidePosition(AbsVector3(0, y, 0));
		if(offside) {
			for(unsigned int i = 0; i < mSupportingPositions[j].size(); i++) {
				mSupportingPositions.at(j).at(i).ShotScore = 0.0f;
				mSupportingPositions.at(j).at(i).PassScore = 0.0f;
			}
			continue;
		}

		for(unsigned int i = 0; i < mSupportingPositions[j].size(); i++) {
			float x = -mMatch->getPitchWidth() * 0.5f + SUPPORTING_POS_RESOLUTION * (i + 2);
			AbsVector3 pos(x, y, 0);
			mSupportingPositions.at(j).at(i).ShotScore =
				calculateShotScoreAt(pos);
			mSupportingPositions.at(j).at(i).PassScore =
				calculatePassScoreAt(offensivePlayers, pos);
		}
	}
}

float Team::calculateShotScoreAt(const AbsVector3& pos) const
{
	float pts = 1.0f;
	float distToGoal = (pos.v - MatchHelpers::oppositeGoalPosition(*this).v).length();
	// std::cout << "Distance to goal: " << distToGoal << "; ";
	pts -= 0.01f * distToGoal;
	assert(pos.v.x > -mMatch->getPitchWidth());
	assert(pos.v.x < mMatch->getPitchWidth());
	assert(pos.v.y > -mMatch->getPitchHeight());
	assert(pos.v.y < mMatch->getPitchHeight());
	if(pts > 0) {
		float distToBall = (pos.v - mMatch->getBall()->getPosition().v).length();
		// corresponds rather directly to how defensive the team plays
		const float optimumDist = 30.0f;
		float coefficient = AIHelpers::scaledDistanceFrom(distToBall, optimumDist);
		if(distToBall > optimumDist || distToGoal > 30.0f)
			pts *= coefficient;
	}

	if(pts > 0) {
		for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
			float distToPl = (pos.v - op->getPosition().v).length();
			if(distToPl < 8.0f) {
				pts -= (8.0 - distToPl) / 16.0f;
				// std::cout << "Distance to player: " << distToPl << "; ";
				if(pts < 0)
					break;
			}
		}
	}
	pts = std::max(0.0f, pts);
	// std::cout << "Total shot points: " << pts << "\n";
	return pts;
}

float Team::calculatePassScoreAt(const std::vector<boost::shared_ptr<Player>>& offensivePlayers,
		const AbsVector3& pos) const
{
	/* TODO: this function returns too often 0 (i.e. when there are no
	 * forwards near the goal). This leads to midfielders standing still.
	 * Make this function richer. */
	float pts = 0.0f;

	for(auto op : offensivePlayers) {
		float distToPl = (pos.v - op->getPosition().v).length();
		const float optimumDist = 20.0f;
		pts += AIHelpers::scaledDistanceFrom(distToPl, optimumDist);
	}
	pts = Common::clamp(0.0f, pts, 1.0f);
	//printf("Pts: %3.3f - ", pts);

	if(pts > 0.0f) {
		for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
			float distToPl = (pos.v - op->getPosition().v).length();
			if(distToPl < 6.0f) {
				pts -= (6.0f - distToPl) / 12.0f;
				// std::cout << "Distance to player: " << distToPl << "; ";
				if(pts < 0.0f)
					break;
			}
		}
		pts = Common::clamp(0.0f, pts, 1.0f);
	}
	// std::cout << "Total pass points: " << pts << "\n";
	//printf("Pts: %3.3f - ", pts);

	if(pts > 0.0) {
		float distToGoal = (pos.v - MatchHelpers::oppositeGoalPosition(*this).v).length();
		const float maxDistToGoal = 30.0f;
		if(distToGoal < maxDistToGoal)
			pts *= distToGoal / maxDistToGoal;
	}
	//printf("Pts: %3.3f\n", pts);
	return pts;
}

void Team::matchHalfChanged(MatchHalf m)
{
	for(auto p : mPlayers) {
		p->matchHalfChanged(m);
	}
}

void Team::setPlayerReceivingPass(Player* p)
{
	mPlayerReceivingPass = p;
}

Player* Team::getPlayerReceivingPass()
{
	return mPlayerReceivingPass;
}

void Team::ballKicked(Player* p)
{
}

bool Team::isOffsidePosition(const AbsVector3& pos) const
{
	int offsideplayers = 0;
	for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
		if((op->getPosition().v.y > pos.v.y) == MatchHelpers::attacksUp(*this))
			offsideplayers++;
	}
	return offsideplayers < 2 && ((mMatch->getBall()->getPosition().v.y < pos.v.y) == MatchHelpers::attacksUp(*this));
}

const AITacticParameters& Team::getAITacticParameters() const
{
	return *mAITacticParameters;
}

