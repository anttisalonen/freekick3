#include "soccer/Team.h"

namespace Soccer {

TeamTactics AITactics::createTeamTactics(const Team& team)
{
	TeamTactics tt;
	/* TODO: add more logic. */
	tt.Pressure    = 0.5f;
	tt.LongBalls   = 0.5f;
	tt.FastPassing = 0.5f;
	tt.ShootClose  = 0.5f;

	// 4-4-2.
	int gk = 0;
	int df = 0;
	int mf = 0;
	int fw = 0;
	for(auto p : team.getPlayers()) {
		PlayerTactics t(0.5, 0.40f);
		switch(p->getPlayerPosition()) {
			case PlayerPosition::Goalkeeper:
				if(gk < 1) {
					tt.mTactics.insert(std::make_pair(p->getId(), t));
					gk++;
				}
				break;

			case PlayerPosition::Defender:
				if(df < 4) {
					if(df == 0) {
						t.WidthPosition = -0.50;
						t.Radius = 0.55f;
					}
					else if(df == 1) {
						t.WidthPosition = -0.20;
						t.Radius = 0.40f;
					}
					else if(df == 2) {
						t.WidthPosition = 0.20;
						t.Radius = 0.40f;
					}
					else {
						t.WidthPosition = 0.50;
						t.Radius = 0.55f;
					}
					tt.mTactics.insert(std::make_pair(p->getId(), t));
					df++;
				}
				break;

			case PlayerPosition::Midfielder:
				if(mf < 4) {
					if(mf == 0) {
						t.WidthPosition = -0.70;
						t.Radius = 0.30f;
					}
					else if(mf == 1) {
						t.WidthPosition = -0.20;
					}
					else if(mf == 2) {
						t.WidthPosition = 0.20;
					}
					else {
						t.WidthPosition = 0.70;
						t.Radius = 0.30f;
					}
					tt.mTactics.insert(std::make_pair(p->getId(), t));
					mf++;
				}
				break;

			case PlayerPosition::Forward:
				if(fw < 2) {
					if(fw == 0) {
						t.WidthPosition = -0.30;
						t.Radius = 0.55f;
					}
					else {
						t.WidthPosition = 0.30;
						t.Radius = 0.55f;
					}
					tt.mTactics.insert(std::make_pair(p->getId(), t));
					fw++;
				}
				break;
		}
		if(gk + df + mf + fw >= 11)
			break;
	}

	return tt;
}

}
