#ifndef SOCCERCOMPETITION_H
#define SOCCERCOMPETITION_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

#include "soccer/Match.h"


namespace Soccer {

class Round {
	public:
		Round() { }
		void addMatch(boost::shared_ptr<Match> m)
		{
			mMatches.push_back(m);
		}
		const std::vector<boost::shared_ptr<Match>>& getMatches() const
		{
			return mMatches;
		}
		const boost::shared_ptr<Match> getMatch(unsigned int rn) const
		{
			if(rn >= mMatches.size())
				return boost::shared_ptr<Match>();
			else
				return mMatches[rn];
		}

		boost::shared_ptr<Match> getMatch(unsigned int rn)
		{
			if(rn >= mMatches.size())
				return boost::shared_ptr<Match>();
			else
				return mMatches[rn];
		}


	private:
		std::vector<boost::shared_ptr<Match>> mMatches;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mMatches;
		}
};

class Schedule {
	public:
		Schedule() { }
		void addRound(const Round& r)
		{
			mRounds.push_back(r);
		}

		const Round* getRound(unsigned int rn) const
		{
			if(rn >= mRounds.size())
				return nullptr;
			else
				return &mRounds[rn];
		}

		Round* getRound(unsigned int rn)
		{
			if(rn >= mRounds.size())
				return nullptr;
			else
				return &mRounds[rn];
		}

	private:
		std::vector<Round> mRounds;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mRounds;
		}
};

/* NOTE: for each class deriving from StatefulCompetition,
 * 1. implement matchPlayed - it should be abstract but isn't due to serialization.
 * 2. add saving the class to YourCompetitionScreen::saveCompetition.
 * 3. add loading the class to LoadGameScreen::buttonPressed.
 */
class StatefulCompetition {
	public:
		StatefulCompetition();
		virtual ~StatefulCompetition() { }
		const Schedule& getSchedule() const;
		const boost::shared_ptr<Match> getNextMatch() const;
		virtual bool matchPlayed(const MatchResult& res) { return false; }
		const Round* getCurrentRound() const;

	protected:
		void setNextMatch();
		Schedule mSchedule;
		boost::shared_ptr<Match> mNextMatch;
	private:
		int mThisRound;
		int mNextMatchId;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mSchedule;
			ar & mNextMatch;
			ar & mThisRound;
			ar & mNextMatchId;
		}
};


}

#endif

