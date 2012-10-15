#ifndef SOCCERCUP_H
#define SOCCERCUP_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

#include "soccer/Competition.h"
#include "soccer/Match.h"


namespace Soccer {

struct CupEntry {
	MatchResult Result;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & Result;
	}
};

class StatefulCup : public StatefulCompetition {
	public:
		StatefulCup(std::vector<boost::shared_ptr<StatefulTeam>>& teams);
		bool matchPlayed(const MatchResult& res) override;
		static std::vector<boost::shared_ptr<Team>> collectTeamsFromCountry(const boost::shared_ptr<LeagueSystem> s);
		virtual CompetitionType getType() const override;
		unsigned int getTotalNumberOfRounds() const;

	private:
		void setupNextRound(std::vector<boost::shared_ptr<StatefulTeam>>& teams);

		std::map<std::pair<boost::shared_ptr<StatefulTeam>, boost::shared_ptr<StatefulTeam>>, CupEntry> mEntries;
		unsigned int mTotalRounds;

		friend class boost::serialization::access;
		StatefulCup(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<StatefulCompetition>(*this);
			ar & mEntries;
			ar & mTotalRounds;
		}
};

}

#endif

