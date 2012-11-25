#ifndef SOCCERCUP_H
#define SOCCERCUP_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

#include "soccer/Competition.h"
#include "soccer/Match.h"


namespace Soccer {

class StatefulLeagueSystem;
class LeagueSystem;
class Team;
class CupEntry;

class StatefulCup : public StatefulCompetition {
	public:
		StatefulCup(std::vector<boost::shared_ptr<StatefulTeam>>& teams, bool onlyoneround = false,
				unsigned int legs = 1, bool awaygoals = false);
		void matchPlayed(const MatchResult& res) override;
		virtual CompetitionType getType() const override;
		unsigned int getTotalNumberOfRounds() const;
		virtual unsigned int getNumberOfTeams() const override;
		virtual std::vector<boost::shared_ptr<StatefulTeam>> getTeamsByPosition() const override;

	private:
		void setupNextRound(std::vector<boost::shared_ptr<StatefulTeam>>& teams);

		std::map<std::pair<boost::shared_ptr<StatefulTeam>, boost::shared_ptr<StatefulTeam>>, CupEntry> mEntries;
		unsigned int mTotalRounds;
		bool mOnlyOneRound;
		unsigned int mLegs;
		bool mAwayGoals;

		friend class boost::serialization::access;
		StatefulCup(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<StatefulCompetition>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatefulCompetition);
			ar & mEntries;
			ar & mTotalRounds;
			ar & mOnlyOneRound;
			ar & mLegs;
			ar & mAwayGoals;
		}
};

unsigned int pow2rounddown(unsigned int x);

template<typename T, typename T2, typename T1>
std::vector<boost::shared_ptr<T1>> collectTeamsFromCountry(boost::shared_ptr<T> s)
{
	std::vector<boost::shared_ptr<T1>> teams;
	unsigned int numTeams = 0;
	for(auto league : s->getLeagues()) {
		numTeams += league->getNumberOfTeams();
	}

	numTeams = pow2rounddown(numTeams);

	auto leagues = s->getLeagues();
	std::sort(leagues.begin(), leagues.end(),
			[](const boost::shared_ptr<T2>& l1, const boost::shared_ptr<T2>& l2) {
			return l1->getLevel() < l2->getLevel(); });

	for(auto league : leagues) {
		if(numTeams == 0)
			break;
		for(auto t : league->getTeamsByPosition()) {
			if(numTeams == 0)
				break;
			teams.push_back(t);
			numTeams--;
		}
	}

	return teams;
}

}

BOOST_CLASS_EXPORT_KEY(Soccer::StatefulCup);


#endif

