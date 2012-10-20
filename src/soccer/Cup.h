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
		StatefulCup(std::vector<boost::shared_ptr<StatefulTeam>>& teams, bool onlyoneround = false);
		void matchPlayed(const MatchResult& res) override;
		static std::vector<boost::shared_ptr<Team>> collectTeamsFromCountry(const boost::shared_ptr<LeagueSystem> s);
		virtual CompetitionType getType() const override;
		unsigned int getTotalNumberOfRounds() const;
		virtual unsigned int getNumberOfTeams() const override;
		virtual std::vector<boost::shared_ptr<StatefulTeam>> getTeamsByPosition() const override;

	private:
		void setupNextRound(std::vector<boost::shared_ptr<StatefulTeam>>& teams);

		std::map<std::pair<boost::shared_ptr<StatefulTeam>, boost::shared_ptr<StatefulTeam>>, CupEntry> mEntries;
		unsigned int mTotalRounds;
		bool mOnlyOneRound;

		friend class boost::serialization::access;
		StatefulCup(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			std::cout << "Serializing cup " << this << "\n";
			ar.template register_type<StatefulCompetition>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatefulCompetition);
			ar & mEntries;
			ar & mTotalRounds;
			ar & mOnlyOneRound;
			std::cout << "Serializing cup done!\n";
		}
};

}

BOOST_CLASS_EXPORT_KEY(Soccer::StatefulCup);


#endif

