#ifndef SOCCER_TOURNAMENT_H
#define SOCCER_TOURNAMENT_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "common/Serialization.h"

#include "soccer/Competition.h"
#include "soccer/League.h"
#include "soccer/Cup.h"


namespace Soccer {

class StatefulTournament;

struct TeamSourceDescriptor {
	unsigned int Teams = 0;
	unsigned int Skip = 0;
	std::string Continent;
	std::string LeagueSystem;
	std::string League;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & Teams;
		ar & Skip;
		ar & Continent;
		ar & LeagueSystem;
		ar & League;
	}
};

class TournamentStage {
	public:
		virtual ~TournamentStage() { }
		TournamentStage() { } // serialization
		virtual void addStage(StatefulTournament& t) { assert(0); }
		virtual unsigned int getTotalTeams() const { assert(0); return 0; }
		virtual unsigned int getContinuingTeams() const { assert(0); return 0; }
		virtual void addTeamSource(const TeamSourceDescriptor& tsd);
		const std::vector<TeamSourceDescriptor>& getTeamSources() const;

	protected:
		std::vector<TeamSourceDescriptor> mTeamSources;

	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mTeamSources;
		}
};

class GroupStage : public TournamentStage {
	public:
		GroupStage(unsigned int numGroups, unsigned int numTeams, unsigned int numWinners, unsigned int legs);
		void addStage(StatefulTournament& t) override;
		unsigned int getTotalTeams() const override;
		unsigned int getContinuingTeams() const override;
		unsigned int getNumberOfLegs() const;
		unsigned int getNumberOfGroups() const;

	private:
		unsigned int mNumGroups;
		unsigned int mNumTeams;
		unsigned int mNumWinners;
		unsigned int mLegs;

		friend class boost::serialization::access;
		GroupStage() { } // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<TournamentStage>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TournamentStage);
			ar & mNumGroups;
			ar & mNumTeams;
			ar & mNumWinners;
			ar & mLegs;
		}
};

class KnockoutStage : public TournamentStage {
	public:
		KnockoutStage(unsigned int legs, bool awaygoals, unsigned int continuingteams);
		void addStage(StatefulTournament& t) override;
		unsigned int getTotalTeams() const override;
		unsigned int getContinuingTeams() const override;
		unsigned int getNumberOfLegs() const;
		bool getAwayGoals() const;

	private:
		unsigned int mContinuingTeams;
		unsigned int mLegs;
		bool mAwayGoals;

		friend class boost::serialization::access;
		KnockoutStage() { } // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<TournamentStage>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TournamentStage);
			ar & mContinuingTeams;
			ar & mLegs;
			ar & mAwayGoals;
		}
};

class TournamentConfig {
	public:
		TournamentConfig(const std::string& tname);
		const std::string& getName() const;
		void pushStage(boost::shared_ptr<TournamentStage> r);
		const std::vector<boost::shared_ptr<TournamentStage>>& getTournamentStages() const;

	private:
		std::vector<boost::shared_ptr<TournamentStage>> mStages;
		std::string mName;

		friend class StatefulTournament;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & mStages;
			ar & mName;
		}
};

class StatefulTournamentStage : public StatefulCompetition {
	public:
		StatefulTournamentStage(const std::vector<boost::shared_ptr<StatefulCompetition>>& p);
		virtual const boost::shared_ptr<Match> getNextMatch() const override;
		virtual void matchPlayed(const MatchResult& res) override;
		virtual CompetitionType getType() const override;
		virtual unsigned int getNumberOfTeams() const override;
		virtual std::vector<boost::shared_ptr<StatefulTeam>> getTeamsByPosition() const override;
		const boost::shared_ptr<StatefulCompetition> getCurrentTournamentGroup() const;
		boost::shared_ptr<StatefulCompetition> getCurrentTournamentGroup();
		const std::vector<boost::shared_ptr<StatefulCompetition>>& getGroups() const;
		virtual std::vector<boost::shared_ptr<Match>> getCurrentRoundMatches() const override;

	private:
		std::vector<boost::shared_ptr<StatefulCompetition>> mTournamentGroups;
		unsigned int mCurrentGroupIndex;

		friend class boost::serialization::access;
		StatefulTournamentStage() { } // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<StatefulCompetition>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatefulCompetition);
			ar & mTournamentGroups;
			ar & mCurrentGroupIndex;
		}
};

class StatefulTournament : public StatefulCompetition {
	public:
		StatefulTournament(const TournamentConfig& tc, std::vector<boost::shared_ptr<StatefulTeam>>& teams);
		virtual const boost::shared_ptr<Match> getNextMatch() const override;
		virtual void matchPlayed(const MatchResult& res) override;
		virtual CompetitionType getType() const override;
		virtual std::vector<boost::shared_ptr<StatefulTeam>> getTeamsByPosition() const override;
		const boost::shared_ptr<StatefulTournamentStage> getCurrentStage() const;
		boost::shared_ptr<StatefulTournamentStage> getCurrentStage();
		virtual std::vector<boost::shared_ptr<Match>> getCurrentRoundMatches() const override;

		void addGroupStage(const GroupStage& r);
		void addKnockoutStage(const KnockoutStage& r);

	private:

		std::vector<boost::shared_ptr<StatefulTournamentStage>> mTournamentStages;
		std::vector<boost::shared_ptr<StatefulTeam>> mTeams;
		TournamentConfig mConfig;

		friend class boost::serialization::access;
		StatefulTournament(); // serialization
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<StatefulCompetition>();
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatefulCompetition);
			ar & mTournamentStages;
			ar & mTeams;
			ar & mConfig;
		}
};

}

BOOST_CLASS_EXPORT_KEY(Soccer::TournamentStage);
BOOST_CLASS_EXPORT_KEY(Soccer::GroupStage);
BOOST_CLASS_EXPORT_KEY(Soccer::KnockoutStage);
BOOST_CLASS_EXPORT_KEY(Soccer::StatefulTournamentStage);
BOOST_CLASS_EXPORT_KEY(Soccer::StatefulTournament);


#endif

