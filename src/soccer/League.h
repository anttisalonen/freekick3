#ifndef SOCCERLEAGUE_H
#define SOCCERLEAGUE_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

#include "soccer/Competition.h"
#include "soccer/Match.h"


namespace Soccer {

struct LeagueEntry {
	LeagueEntry()
		: Points(0),
		GoalsFor(0),
		GoalsAgainst(0),
		Matches(0),
		Wins(0),
		Draws(0),
		Losses(0) { }
	int Points;
	int GoalsFor;
	int GoalsAgainst;
	int Matches;
	int Wins;
	int Draws;
	int Losses;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & Points;
		ar & GoalsFor;
		ar & GoalsAgainst;
		ar & Matches;
		ar & Wins;
		ar & Draws;
		ar & Losses;
	}
};

class StatefulLeague : public StatefulCompetition {
	public:
		StatefulLeague(std::vector<boost::shared_ptr<StatefulTeam>>& teams, unsigned int numCycles = 2);
		const std::map<boost::shared_ptr<StatefulTeam>, LeagueEntry>& getEntries() const;
		void matchPlayed(const MatchResult& res) override;
		virtual CompetitionType getType() const override;
		virtual unsigned int getNumberOfTeams() const override;
		virtual std::vector<boost::shared_ptr<StatefulTeam>> getTeamsByPosition() const override;

	private:
		void setRoundRobin(std::vector<boost::shared_ptr<StatefulTeam>>& teams);
		std::map<boost::shared_ptr<StatefulTeam>, LeagueEntry> mEntries;
		const int mPointsPerWin;
		const int mNumCycles;

		friend class boost::serialization::access;
		StatefulLeague(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<StatefulCompetition>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatefulCompetition);
			ar & mEntries;
			ar & const_cast<int&>(mPointsPerWin);
			ar & const_cast<int&>(mNumCycles);
		}
};

}

BOOST_CLASS_EXPORT_KEY(Soccer::StatefulLeague);

#endif

