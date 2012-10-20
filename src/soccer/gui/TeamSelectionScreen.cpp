#include "soccer/gui/TeamSelectionScreen.h"
#include "soccer/gui/TournamentScreen.h"

namespace Soccer {

TeamSelectionScreen::TeamSelectionScreen(boost::shared_ptr<ScreenManager> sm,
		const std::vector<boost::shared_ptr<Team>>& teams,
		std::unique_ptr<TournamentConfig> tc)
	: Screen(sm),
	mTournamentConfig(std::move(tc))
{
	addButton("Back", Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	mPlayButton = addButton("Play", Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));
	updatePlayButton();

	int maxnum = teams.size();
	int i = 0;
	for(auto c : teams) {
		const std::string& tname = c->getName();
		auto b = TeamBrowser::createSelectionButton(*this, tname.c_str(), i, maxnum);
		mTeams.insert({c, TeamSelection::Computer});
		mButtons.insert({tname, c});
		TeamBrowser::setTeamButtonColor(b, TeamSelection::Computer);
		i++;
	}
}

void TeamSelectionScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Play") {
		clickedDone();
	} else if(buttonText == "Back") {
		mScreenManager->dropScreen();
	} else {
		auto it = mButtons.find(buttonText);
		if(it != mButtons.end()) {
			auto it2 = mTeams.find(it->second);
			assert(it2 != mTeams.end());
			if(it2 != mTeams.end()) {
				teamClicked(it->second);
				TeamBrowser::setTeamButtonColor(button, it2->second);
				updatePlayButton();
			}
		}
	}
}

void TeamSelectionScreen::updatePlayButton()
{
	mPlayButton->hide();
	if(canClickDone()) {
		mPlayButton->show();
	}
}

bool TeamSelectionScreen::canClickDone() const
{
	return true;
}

void TeamSelectionScreen::clickedDone()
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;

	teams = TeamBrowser::createStatefulTeams(mTeams);
	std::random_shuffle(teams.begin(), teams.end());

	boost::shared_ptr<StatefulTournament> tournament(new StatefulTournament(*mTournamentConfig, teams));
	mScreenManager->addScreen(boost::shared_ptr<Screen>(new TournamentScreen(mScreenManager, tournament)));
}

const std::string TeamSelectionScreen::ScreenName = std::string("Team Selection Screen");

const std::string& TeamSelectionScreen::getName() const
{
	return ScreenName;
}

void TeamSelectionScreen::teamClicked(boost::shared_ptr<Team> p)
{
	TeamBrowser::toggleTeamOwnership(p, mTeams);
}

}
