#ifndef SOCCERSEASON_H
#define SOCCERSEASON_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Serialization.h"

#include "soccer/League.h"
#include "soccer/Cup.h"
#include "soccer/Tournament.h"
#include "soccer/Match.h"


namespace Soccer {

class StatefulLeagueSystem {
	public:
		StatefulLeagueSystem();
		void addLeague(boost::shared_ptr<StatefulLeague> l);
		void setCup(boost::shared_ptr<StatefulCup> c);
		std::vector<boost::shared_ptr<StatefulLeague>>& getLeagues();
		void promoteAndRelegateTeams();

	private:
		std::vector<boost::shared_ptr<StatefulLeague>> mLeagues;
		boost::shared_ptr<StatefulCup> mCup;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mLeagues;
		}
};

class Season {
	public:
		Season(boost::shared_ptr<StatefulTeam> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c,
				boost::shared_ptr<StatefulLeagueSystem> ls);
		void reset(boost::shared_ptr<StatefulTeam> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c,
				boost::shared_ptr<StatefulLeagueSystem> ls);
		static boost::shared_ptr<Season> createSeason(boost::shared_ptr<StatefulTeam> plteam,
				boost::shared_ptr<StatefulLeagueSystem> country);
		static boost::shared_ptr<Season> createSeason(boost::shared_ptr<Team> plteam,
				boost::shared_ptr<LeagueSystem> country, bool addSystem);
		boost::shared_ptr<StatefulTeam> getTeam();
		boost::shared_ptr<StatefulLeague> getLeague();
		boost::shared_ptr<StatefulCup> getCup();
		boost::shared_ptr<StatefulTournament> getTournament();
		boost::shared_ptr<StatefulLeagueSystem> getLeagueSystem();
		const std::vector<std::pair<CompetitionType, unsigned int>>& getSchedule() const;

	private:
		void createSchedule();

		boost::shared_ptr<StatefulTeam> mTeam;
		boost::shared_ptr<StatefulLeague> mLeague;
		boost::shared_ptr<StatefulCup> mCup;
		boost::shared_ptr<StatefulTournament> mTournament;
		boost::shared_ptr<StatefulLeagueSystem> mLeagueSystem;

		std::vector<std::pair<CompetitionType, unsigned int>> mSchedule;

		friend class boost::serialization::access;
		Season(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mTeam;
			ar & mLeague;
			ar & mCup;
			ar & mTournament;
			ar & mLeagueSystem;
			ar & mSchedule;
		}
};

}

#endif

