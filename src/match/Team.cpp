#include <assert.h>

#include "common/Math.h"

#include "match/Team.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

#define SUPPORTING_POS_RESOLUTION 4

using Common::Vector3;

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

void Team::getSupportPositionCoordinates(const Vector3& pos, unsigned int& i, unsigned int& j) const
{
	i = std::max(0, (int)(pos.x + mMatch->getPitchWidth() * 0.5f) / SUPPORTING_POS_RESOLUTION - 2);
	j = std::max(0, (int)(pos.y + mMatch->getPitchHeight() * 0.5f) / SUPPORTING_POS_RESOLUTION - 2);
	if(j >= mSupportingPositions.size())
		j = mSupportingPositions.size() - 1;
	if(i >= mSupportingPositions[j].size())
		i = mSupportingPositions[j].size() - 1;
	// printf("Support coords: (%3.1f, %3.1f) => (%d, %d) = %3.3f\n", pos.x, pos.y, i, j, mSupportingPositions[j][i]);
}

float Team::getShotScoreAt(const Vector3& pos) const
{
	unsigned int i, j;
	getSupportPositionCoordinates(pos, i, j);
	return mSupportingPositions.at(j).at(i).ShotScore;
}

float Team::getPassScoreAt(const Vector3& pos) const
{
	unsigned int i, j;
	getSupportPositionCoordinates(pos, i, j);
	return mSupportingPositions.at(j).at(i).PassScore;
}

void Team::updateSupportingPositions()
{
	std::vector<boost::shared_ptr<Player>> passTargetCandidates;
	Vector3 oppgoal = MatchHelpers::oppositeGoalPosition(*this);
	for(auto pl : mPlayers) {
		float goaldist = (pl->getPosition() - oppgoal).length();
		if((goaldist < 60.0f && pl->getPlayerPosition() == Soccer::PlayerPosition::Forward) ||
			(goaldist < 45.0f)) {
			passTargetCandidates.push_back(pl);
		}
	}

	for(unsigned int j = 0; j < mSupportingPositions.size(); j++) {
		float y = -mMatch->getPitchHeight() * 0.5f + SUPPORTING_POS_RESOLUTION * (j + 2);

		bool offside = isOffsidePosition(Vector3(0, y, 0));
		if(offside) {
			for(unsigned int i = 0; i < mSupportingPositions[j].size(); i++) {
				mSupportingPositions.at(j).at(i).ShotScore = 0.0f;
				mSupportingPositions.at(j).at(i).PassScore = 0.0f;
			}
			continue;
		}

		for(unsigned int i = 0; i < mSupportingPositions[j].size(); i++) {
			float x = -mMatch->getPitchWidth() * 0.5f + SUPPORTING_POS_RESOLUTION * (i + 2);
			Vector3 pos(x, y, 0);
			mSupportingPositions.at(j).at(i).ShotScore =
				calculateShotScoreAt(pos);
		}
	}

	/* calculate shot score first as it will be required by the pass score */
	for(unsigned int j = 0; j < mSupportingPositions.size(); j++) {
		float y = -mMatch->getPitchHeight() * 0.5f + SUPPORTING_POS_RESOLUTION * (j + 2);
		for(unsigned int i = 0; i < mSupportingPositions[j].size(); i++) {
			float x = -mMatch->getPitchWidth() * 0.5f + SUPPORTING_POS_RESOLUTION * (i + 2);
			Vector3 pos(x, y, 0);
			mSupportingPositions.at(j).at(i).PassScore =
				calculatePassScoreAt(passTargetCandidates, pos);
		}
	}
}

float Team::calculateShotScoreAt(const Vector3& pos) const
{
	float pts = 1.0f;
	float distToGoal = (pos - MatchHelpers::oppositeGoalPosition(*this)).length();
	// std::cout << "Distance to goal: " << distToGoal << "; ";
	pts -= 0.01f * distToGoal;
	assert(pos.x > -mMatch->getPitchWidth());
	assert(pos.x < mMatch->getPitchWidth());
	assert(pos.y > -mMatch->getPitchHeight());
	assert(pos.y < mMatch->getPitchHeight());
	float distToGoalCoeff = std::min(1.0f, AIHelpers::scaledCoefficient(distToGoal, 30.0f) + 0.25f);

	if(pts > 0) {
		for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
			float distToOpp = (pos - op->getPosition()).length();
			const float maxDistToOpp = 8.0f;
			if(distToOpp < maxDistToOpp) {
				pts -= (1.0f - distToGoalCoeff) * AIHelpers::scaledCoefficient(distToOpp, maxDistToOpp);
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
		const Vector3& pos) const
{
	float pts = 0.0f;

	float depthCoeff = sqrt(AIHelpers::getDepthCoefficient(*this, pos));
	for(auto op : offensivePlayers) {
		if((pos.y > op->getPosition().y) == MatchHelpers::attacksUp(*this))
			continue;

		float shotScore = getShotScoreAt(pos);
		float distToPl = (pos - op->getPosition()).length();
		float optimumDist = 20.0f;
		float distScore = AIHelpers::scaledDistanceFrom(distToPl, optimumDist);
		pts += (distScore + shotScore) * depthCoeff;
	}

	if(pts > 0.0f) {
		for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
			float distToOpp = (pos - op->getPosition()).length();
			const float maxDistToOpp = 10.0f;
			if(distToOpp < maxDistToOpp) {
				pts *= (1 - depthCoeff) * AIHelpers::scaledCoefficient(distToOpp, maxDistToOpp);
				if(pts < 0.0f)
					break;
			}
		}
	}

	if(mMatch->getPlayState() == PlayState::OutThrowin) {
		float distToBall = (pos - mMatch->getBall()->getPosition()).length();
		pts *= AIHelpers::scaledCoefficient(distToBall, 50.0f);
	}

	pts = Common::clamp(0.0f, pts, 1.0f);
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

bool Team::isOffsidePosition(const Vector3& pos) const
{
	int offsideplayers = 0;
	for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
		if((op->getPosition().y > pos.y) == MatchHelpers::attacksUp(*this))
			offsideplayers++;
	}
	return offsideplayers < 2 && ((mMatch->getBall()->getPosition().y < pos.y) == MatchHelpers::attacksUp(*this));
}

const AITacticParameters& Team::getAITacticParameters() const
{
	return *mAITacticParameters;
}

