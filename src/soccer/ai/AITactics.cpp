#include <assert.h>

#include "common/Math.h"

#include "soccer/Team.h"

#define TACTICS_DEBUG

namespace Soccer {

PlayerSkillValue AITactics::calculatePlayerSkill(const Player& p)
{
	PlayerSkillValue ps;

	ps.Goalkeeping = 5.0f * p.getSkills().GoalKeeping;
	ps.Defending = 2.0f * p.getSkills().Tackling +
		p.getSkills().BallControl +
		p.getSkills().Passing +
		p.getSkills().RunSpeed;
	ps.Midfield = p.getSkills().Tackling +
		p.getSkills().BallControl +
		2.0f * p.getSkills().Passing +
		p.getSkills().RunSpeed;
	ps.Forward = 2.0f * p.getSkills().ShotPower +
		p.getSkills().RunSpeed +
		p.getSkills().BallControl +
		p.getSkills().Heading;

#ifdef TACTICS_DEBUG
	printf("%30s %3.4f %3.4f %3.4f %3.4f\n", p.getName().c_str(),
			ps.Goalkeeping, ps.Defending,
			ps.Midfield, ps.Forward);
#endif

	return ps;
}

TeamTactics AITactics::createTeamTactics(const Team& team, unsigned int def, unsigned int mid, unsigned int forw)
{
	TeamTactics tt;

	typedef std::pair<boost::shared_ptr<Player>, PlayerSkillValue> Item;

	std::vector<Item> skillvalues;

#ifdef TACTICS_DEBUG
	printf("%30s %6s %6s %6s %6s\n", "Name", "GK", "DF", "MF", "FW");
#endif

	for(auto p : team.getPlayers()) {
		skillvalues.push_back(std::make_pair(p, calculatePlayerSkill(*p)));
	}
	assert(skillvalues.size() >= 11);

	std::sort(skillvalues.begin(), skillvalues.end(), [](const Item& i1, const Item& i2) {
			return i1.second.Goalkeeping > i1.second.Goalkeeping; });

	boost::shared_ptr<Player> goalkeeper = skillvalues[0].first;
	std::vector<boost::shared_ptr<Player>> defenders, midfielders, forwards;
	skillvalues.erase(skillvalues.begin());

	int defslots;
	int mfslots;
	int fwslots;
	bool freespace;
	if(def + mid + forw == 10) {
		defslots = def;
		mfslots = mid;
		fwslots = forw;
		freespace = false;
	}
	else {
		defslots = 5;
		mfslots = 5;
		fwslots = 3;
		freespace = true;
	}

	for(int i = 0; i < 10; i++) {
		if(freespace && defenders.size() == 5) {
			mfslots = 4 - midfielders.size();
			fwslots = 2 - forwards.size();
		}
		if(freespace && midfielders.size() == 5) {
			defslots = 4 - defenders.size();
			fwslots = 2 - forwards.size();
		}
		if(freespace && forwards.size() == 3) {
			defslots = 4 - defenders.size();
			mfslots = 4 - midfielders.size();
		}

		float highest = 0.0f;
		int type = -1;
		std::vector<Item>::iterator bestplayer = skillvalues.end();
		for(auto p = skillvalues.begin(); p != skillvalues.end(); p++) {
			if(fwslots > 0 && p->second.Forward > highest) {
				highest = p->second.Forward;
				type = 3;
				bestplayer = p;
			}
			if(mfslots > 0 && p->second.Midfield > highest) {
				highest = p->second.Midfield;
				type = 2;
				bestplayer = p;
			}
			if(defslots > 0 && p->second.Defending > highest) {
				highest = p->second.Defending;
				type = 1;
				bestplayer = p;
			}
		}

		assert(bestplayer != skillvalues.end());

		switch(type) {
			case 1:
				defenders.push_back(bestplayer->first);
				defslots--;
#ifdef TACTICS_DEBUG
				std::cout << "Picked DF " << bestplayer->first->getName() << "\n";
#endif
				break;

			case 2:
				midfielders.push_back(bestplayer->first);
				mfslots--;
#ifdef TACTICS_DEBUG
				std::cout << "Picked MF " << bestplayer->first->getName() << "\n";
#endif
				break;

			default:
				forwards.push_back(bestplayer->first);
				fwslots--;
#ifdef TACTICS_DEBUG
				std::cout << "Picked FW " << bestplayer->first->getName() << "\n";
#endif
				break;
		}
		skillvalues.erase(bestplayer);
	}

	tt.mTactics.insert(std::make_pair(goalkeeper->getId(), PlayerTactics(0.0, 0.40f, PlayerPosition::Goalkeeper, false)));

	/* TODO: add logic to decide whether the player should be offensive. */
	/* TODO: add logic to separate wing players from central players. */
	unsigned int pos = 0;
	for(auto p : defenders) {
		PlayerTactics t = createPlayerTactics(PlayerPosition::Defender, pos, defenders.size(), false);
		tt.mTactics.insert(std::make_pair(p->getId(), t));
		pos++;
	}

	pos = 0;
	for(auto p : midfielders) {
		PlayerTactics t = createPlayerTactics(PlayerPosition::Midfielder, pos, midfielders.size(), false);
		tt.mTactics.insert(std::make_pair(p->getId(), t));
		pos++;
	}

	pos = 0;
	for(auto p : forwards) {
		PlayerTactics t = createPlayerTactics(PlayerPosition::Forward, pos, forwards.size(), false);
		tt.mTactics.insert(std::make_pair(p->getId(), t));
		pos++;
	}

#ifdef TACTICS_DEBUG
	std::cout << team.getName() << " playing with " << defenders.size() << "-" <<
		midfielders.size() << "-" << forwards.size() << ".\n";
#endif

	assert(tt.mTactics.size() == 11);
	assert(defenders.size() >= 3 && defenders.size() <= 5);
	assert(midfielders.size() >= 3 && midfielders.size() <= 5);
	assert(forwards.size() >= 1 && forwards.size() <= 3);

	/* TODO: add more logic. */
	tt.Pressure    = 0.5f;
	tt.LongBalls   = 0.5f;
	tt.FastPassing = 0.5f;
	tt.ShootClose  = 0.5f;

	float passavg = 0.0f;
	float headingavg = 0.0f;
	float controlavg = 0.0f;
	float shotavg = 0.0f;
	float tacklingavg = 0.0f;
	float speedavg = 0.0f;
	std::vector<boost::shared_ptr<Player>> allplayers;
	allplayers.insert(allplayers.end(), defenders.begin(), defenders.end());
	allplayers.insert(allplayers.end(), midfielders.begin(), midfielders.end());
	allplayers.insert(allplayers.end(), forwards.begin(), forwards.end());
	for(auto p : allplayers) {
		passavg += p->getSkills().Passing;
		headingavg += p->getSkills().Heading;
		controlavg += p->getSkills().BallControl;
		shotavg += p->getSkills().ShotPower;
		tacklingavg += p->getSkills().Tackling;
		speedavg += p->getSkills().RunSpeed;
	}

	float totalavg = passavg + headingavg + controlavg + shotavg + tacklingavg + speedavg;
	totalavg /= 6.0f;
	totalavg /= allplayers.size();
	passavg /= allplayers.size();
	headingavg /= allplayers.size();
	controlavg /= allplayers.size();
	shotavg /= allplayers.size();
	tacklingavg /= allplayers.size();
	speedavg /= allplayers.size();

	float passdiff = passavg - totalavg;
	float headdiff = headingavg - totalavg;
	float contdiff = controlavg - totalavg;
	float shotdiff = shotavg - totalavg;
	float tackdiff = tacklingavg - totalavg;
	float speddiff = speedavg - totalavg;

	tt.Pressure    = Common::clamp(0.0f, 0.5f +  speddiff * 5.0f +  tackdiff * 5.0f, 1.0f);
	tt.LongBalls   = Common::clamp(0.0f, 0.5f + -passdiff * 5.0f +  headdiff * 5.0f, 1.0f);
	tt.FastPassing = Common::clamp(0.0f, 0.5f +  passdiff * 5.0f + -contdiff * 5.0f, 1.0f);
	tt.ShootClose  = Common::clamp(0.0f, 0.5f + -shotdiff * 5.0f +  speddiff * 5.0f, 1.0f);

#ifdef TACTICS_DEBUG
	printf("Passing:        %3.4f - %3.4f\n", passavg, passdiff);
	printf("Heading:        %3.4f - %3.4f\n", headingavg, headdiff);
	printf("Control:        %3.4f - %3.4f\n", controlavg, contdiff);
	printf("Shooting:       %3.4f - %3.4f\n", shotavg, shotdiff);
	printf("Tackling:       %3.4f - %3.4f\n", tacklingavg, tackdiff);
	printf("Speed:          %3.4f - %3.4f\n", speedavg, speddiff);
	printf("Pressure:       %3.4f\n", tt.Pressure);
	printf("Long balls:     %3.4f\n", tt.LongBalls);
	printf("Fast passing:   %3.4f\n", tt.FastPassing);
	printf("Close shooting: %3.4f\n", tt.ShootClose);
#endif

	return tt;
}

TeamTactics AITactics::updateTeamTactics(const TeamTactics& t, unsigned int def,
		unsigned int mid, unsigned int forw)
{
	TeamTactics tt(t);

	tt.mTactics.clear();

	assert(def + mid + forw == 10);
	assert(t.mTactics.size() == 11);

	unsigned int goalkeepers = 0;
	unsigned int defenders = 0;
	unsigned int midfielders = 0;
	unsigned int forwards = 0;

	for(auto p : t.mTactics) {
		PlayerPosition postoadd = p.second.Position;
		unsigned int num = 0;
		unsigned int total = 0;
		bool already_retried = false;

		switch(postoadd) {
retry:
			already_retried = true;
			case PlayerPosition::Goalkeeper:
				if(goalkeepers < 1) {
					num = goalkeepers++;
					total = 1;
					break;
				}
				// FALL THROUGH

			case PlayerPosition::Defender:
				if(defenders != def) {
					num = defenders++;
					postoadd = PlayerPosition::Defender;
					total = def;
					break;
				}
				// FALL THROUGH

			case PlayerPosition::Midfielder:
				if(midfielders != mid) {
					num = midfielders++;
					postoadd = PlayerPosition::Midfielder;
					total = mid;
					break;
				}
				// FALL THROUGH

			case PlayerPosition::Forward:
				if(forwards != forw) {
					num = forwards++;
					postoadd = PlayerPosition::Forward;
					total = forw;
					break;
				}
				else {
					if(already_retried) {
						assert(0);
						throw std::runtime_error("updateTeamTactics: could not find player position!");
					}
					goto retry;
				}
		}

		tt.mTactics.insert(std::make_pair(p.first, createPlayerTactics(postoadd,
						num, total, p.second.Offensive)));
	}

	assert(tt.mTactics.size() == 11);

	return tt;
}

PlayerTactics AITactics::createPlayerTactics(Soccer::PlayerPosition pos,
		unsigned int num, unsigned int howmany, bool offensive)
{
	switch(pos) {
		case PlayerPosition::Goalkeeper:
			return PlayerTactics(0.0f, 0.40f, PlayerPosition::Goalkeeper, false);

		case PlayerPosition::Defender:
			{
				PlayerTactics t(0.0f, 0.40f, PlayerPosition::Defender, offensive);
				if(num == 0 || num == howmany - 1)
					t.Radius = 0.55f;

				t.WidthPosition = (num + 1) / ((float)(howmany + 1.0f)) * 2.0f - 1.0f;
				return t;
			}

		case PlayerPosition::Midfielder:
			{
				PlayerTactics t(0.0f, 0.40f, PlayerPosition::Midfielder, offensive);

				t.WidthPosition = (num + 1) / ((float)(howmany + 1.0f)) * 2.0f - 1.0f;
				return t;
			}

		case PlayerPosition::Forward:
			{
				PlayerTactics t(0.0f, 0.55f, PlayerPosition::Forward, offensive);

				t.WidthPosition = (num + 1) / ((float)(howmany + 1.0f)) * 2.0f - 1.0f;
				return t;
			}
	}
	assert(0);
	throw std::runtime_error("Error in createPlayerTactics: player position not handled\n");
}

}
