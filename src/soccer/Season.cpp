#include "Season.h"

namespace Soccer {

Season::Season(boost::shared_ptr<StatefulTeam> t, boost::shared_ptr<StatefulLeague> l, boost::shared_ptr<StatefulCup> c)
	: mTeam(t),
	mLeague(l),
	mCup(c)
{
	createSchedule();
}

Season::Season()
{
}

boost::shared_ptr<StatefulTeam> Season::getTeam()
{
	return mTeam;
}

boost::shared_ptr<StatefulLeague> Season::getLeague()
{
	return mLeague;
}

boost::shared_ptr<StatefulCup> Season::getCup()
{
	return mCup;
}

boost::shared_ptr<StatefulTournament> Season::getTournament()
{
	return mTournament;
}

void Season::createSchedule()
{
	mSchedule.clear();

	unsigned int cupRounds = mCup->getTotalNumberOfRounds();
	unsigned int leagueRounds = mLeague->getSchedule().getNumberOfRounds();
	unsigned int totalCupRounds = cupRounds;
	unsigned int totalLeagueRounds = leagueRounds;

	if(!leagueRounds) {
		for(unsigned int i = 0; i < cupRounds; i++)
			mSchedule.push_back({CompetitionType::Cup, i});

		return;
	}

	if(!cupRounds) {
		for(unsigned int i = 0; i < leagueRounds; i++)
			mSchedule.push_back({CompetitionType::League, i});

		return;
	}

	float ratio = leagueRounds/(float)cupRounds;
	float counter = 0.0f;

	while(cupRounds || leagueRounds) {
		counter += ratio;
		while(counter > 0.0f) {
			if(leagueRounds) {
				mSchedule.push_back({CompetitionType::League, totalLeagueRounds - leagueRounds});
				leagueRounds--;
			}
			counter -= 1.0f;
		}
		if(cupRounds) {
			mSchedule.push_back({CompetitionType::Cup, totalCupRounds - cupRounds});
			cupRounds--;
		}
	}
}

const std::vector<std::pair<CompetitionType, unsigned int>>& Season::getSchedule() const
{
	return mSchedule;
}


}

