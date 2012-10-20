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

		unsigned int getNumberOfRounds() const
		{
			return mRounds.size();
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

enum class CompetitionType {
	League,
	Cup,
	Tournament
};

/* NOTE: for each class deriving from StatefulCompetition,
 * 1. implement matchPlayed, getType, getNumberOfTeams and getTeamsByPosition -
 *    they should be abstract but aren't due to serialization.
 * 2. In the constructor, add at least one round with matches in the schedule and call setNextMatch().
 * 3. add saving the class to YourCompetitionScreen::saveCompetition.
 * 4. add loading the class to LoadGameScreen::buttonPressed.
 * 5. in matchPlayed(), you must call setNextMatch(). You can add also matches to the schedule in matchPlayed().
 */
class StatefulCompetition {
	public:
		StatefulCompetition();
		virtual ~StatefulCompetition() { }
		const Schedule& getSchedule() const;
		virtual const boost::shared_ptr<Match> getNextMatch() const;
		/* The match played is mNextMatch with its result set to res.
		 * To add matches the callback should use mSchedule.addRound(). */
		virtual void matchPlayed(const MatchResult& res) { assert(0); }
		virtual CompetitionType getType() const { assert(0); return CompetitionType::League; }
		virtual const Round* getCurrentRound() const;
		virtual unsigned int getNumberOfTeams() const { assert(0); return 0; }
		virtual std::vector<boost::shared_ptr<StatefulTeam>> getTeamsByPosition() const {
			assert(0); return std::vector<boost::shared_ptr<StatefulTeam>>();
		}

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

BOOST_CLASS_EXPORT_KEY(Soccer::StatefulCompetition);

#endif

