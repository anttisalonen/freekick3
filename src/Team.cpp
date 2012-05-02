#include <assert.h>

#include "Team.h"
#include "MatchHelpers.h"

#define SUPPORTING_POS_RESOLUTION 4

Team::Team(Match* match, bool first)
	: mMatch(match),
	mFirst(first),
	mPlayerNearestToBall(nullptr),
	mSupportingPositionsTimer(0.5f)
{
	for(unsigned int j = SUPPORTING_POS_RESOLUTION * 2;
			j <= match->getPitchHeight() - SUPPORTING_POS_RESOLUTION;
			j += SUPPORTING_POS_RESOLUTION) {
		mSupportingPositions.push_back(std::vector<float>());
		for(unsigned int i = SUPPORTING_POS_RESOLUTION * 2;
				i <= match->getPitchWidth() - SUPPORTING_POS_RESOLUTION;
				i += SUPPORTING_POS_RESOLUTION * 2) {
			mSupportingPositions[mSupportingPositions.size() - 1].push_back(0.0f);
		}
	}
}

void Team::addPlayer()
{
	std::shared_ptr<Player> p(new Player(mMatch, this, mPlayers.size() == 0));
	mPlayers.push_back(p);
	if(mPlayers.size() == 1) {
		p->setHomePosition(RelVector3(0, -0.95f * (mFirst ? 1 : -1), 0));
	}
	else {
		int hgt = (mPlayers.size() - 2) / 4;
		int widx = (mPlayers.size() - 2) % 4;
		if(hgt >= 2) {
			if(mFirst) {
				p->setHomePosition(RelVector3(widx < 1 ? -0.1f : 0.1f,
							-0.01f, 0));
			}
			else {
				p->setHomePosition(RelVector3(widx < 1 ? -0.1f : 0.1f,
							0.15f, 0));
			}
		}
		else {
			float wdt = (widx - 1.5f) * 0.5f;
			p->setHomePosition(RelVector3(wdt, (mFirst ? 1 : -1) * -0.7f + hgt * 0.3f * (mFirst ? 1 : -1), 0));
		}
	}

	if(mPlayers.size() > 5) {
		p->setPlayerTactics(PlayerTactics(true));
	}
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

const std::vector<std::shared_ptr<Player>>& Team::getPlayers() const
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
		mPlayerNearestToBall = MatchHelpers::nearestOwnPlayerToBall(*this);
}

float Team::getSupportingPositionScoreAt(const AbsVector3& pos) const
{
	unsigned int i = std::max(0, (int)(pos.v.x + mMatch->getPitchWidth() * 0.5f) / SUPPORTING_POS_RESOLUTION - 2);
	unsigned int j = std::max(0, (int)(pos.v.y + mMatch->getPitchHeight() * 0.5f) / SUPPORTING_POS_RESOLUTION - 2);
	if(j >= mSupportingPositions.size())
		j = mSupportingPositions.size() - 1;
	if(i >= mSupportingPositions[j].size())
		i = mSupportingPositions[j].size() - 1;
	// printf("Support coords: (%3.1f, %3.1f) => (%d, %d) = %3.3f\n", pos.v.x, pos.v.y, i, j, mSupportingPositions[j][i]);
	return mSupportingPositions.at(j).at(i);
}

void Team::updateSupportingPositions()
{
	for(unsigned int j = 0; j < mSupportingPositions.size(); j++) {
		for(unsigned int i = 0; i < mSupportingPositions[j].size(); i++) {
			mSupportingPositions.at(j).at(i) =
				calculateSupportingPositionScoreAt(AbsVector3(-mMatch->getPitchWidth() * 0.5f +
							SUPPORTING_POS_RESOLUTION * (i + 2),
							-mMatch->getPitchHeight() * 0.5f +
							SUPPORTING_POS_RESOLUTION * (j + 2), 0));
		}
	}
}

float Team::calculateSupportingPositionScoreAt(const AbsVector3& pos) const
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
		if(distToBall < 5.0f || distToBall > 40.0f) {
			pts = 0;
		}
	}
	int offsideplayers = 0;
	if(pts > 0) {
		for(auto op : MatchHelpers::getOpposingPlayers(*this)) {
			float distToPl = (pos.v - op->getPosition().v).length();
			if((op->getPosition().v.y >= pos.v.y) == MatchHelpers::attacksUp(*this))
				offsideplayers++;
			if(distToPl < 8.0f) {
				pts -= (8.0 - distToPl) / 16.0f;
				// std::cout << "Distance to player: " << distToPl << "; ";
				if(pts < 0)
					break;
			}
		}
	}
	if(offsideplayers < 2 && (mMatch->getBall()->getPosition().v.y < pos.v.y) == MatchHelpers::attacksUp(*this))
		pts = 0.0f;
	else
		pts = std::max(0.0f, pts);
	// std::cout << "Total points: " << pts << "\n";
	return pts;
}


