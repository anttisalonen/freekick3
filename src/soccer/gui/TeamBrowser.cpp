#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/TeamBrowser.h"
#include "soccer/gui/Menu.h"

namespace Soccer {

TeamBrowser::TeamBrowser(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm),
	mCurrentContinent(boost::shared_ptr<Continent>()),
	mCurrentCountry(boost::shared_ptr<LeagueSystem>()),
	mCurrentLeague(boost::shared_ptr<League>()),
	mCurrentLevel(TeamBrowserLevel::Continents)
{
	addButton("Back", Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	mPlayButton = addButton("Play", Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));
	mPlayButton->hide();

	addContinentButtons();
}

boost::shared_ptr<Button> TeamBrowser::createSelectionButton(Screen& scr, const char* text, int i, int maxnum)
{
	if(i > maxnum)
		return boost::shared_ptr<Button>();

	if(maxnum < 10) {
		boost::shared_ptr<Button> b(scr.addButton(text, Common::Rectangle(0.35f, 0.05f + i * 0.08f, 0.25f, 0.05f)));
		return b;
	}
	else if(maxnum < 30) {
		boost::shared_ptr<Button> b(scr.addButton(text, Common::Rectangle(0.05f + (i % 3) * 0.30f,
						0.05f + (i / 3) * 0.08f, 0.25f, 0.05f)));
		return b;
	}
	else if(maxnum < 50) {
		boost::shared_ptr<Button> b(scr.addButton(text, Common::Rectangle(0.05f + (i % 3) * 0.30f,
						0.05f + (i / 3) * 0.05f, 0.25f, 0.04f)));
		return b;
	}
	else {
		if(i > 71)
			return boost::shared_ptr<Button>();

		boost::shared_ptr<Button> b(scr.addButton(text, Common::Rectangle(0.05f + (i % 3) * 0.30f,
						0.05f + (i / 3) * 0.035f, 0.25f, 0.03f)));
		return b;
	}
}

void TeamBrowser::addSelectionButton(const char* text, int i, int maxnum)
{
	auto b = createSelectionButton(*this, text, i, maxnum);
	if(b)
		mCurrentButtons.push_back(b);
}

void TeamBrowser::addContinentButtons()
{
	clearCurrentButtons();
	mContinentButtons.clear();
	int maxnum = mScreenManager->getTeamDatabase().getContainer().size();
	int i = 0;
	for(auto c : mScreenManager->getTeamDatabase().getContainer()) {
		const std::string& tname = c.second->getName();
		mContinentButtons.insert(std::make_pair(tname, c.second));
		addSelectionButton(tname.c_str(), i, maxnum);
		i++;
	}
	mCurrentLevel = TeamBrowserLevel::Continents;
}

void TeamBrowser::addCountryButtons(boost::shared_ptr<Continent> c)
{
	clearCurrentButtons();
	mCountryButtons.clear();
	int maxnum = c->getContainer().size();
	int i = 0;
	for(auto country : c->getContainer()) {
		const std::string& tname = country.second->getName();
		mCountryButtons.insert(std::make_pair(tname, country.second));
		addSelectionButton(tname.c_str(), i, maxnum);
		i++;
	}
	mCurrentLevel = TeamBrowserLevel::Countries;
	mCurrentContinent = c;
}

void TeamBrowser::addLeagueButtons(boost::shared_ptr<LeagueSystem> c)
{
	clearCurrentButtons();
	mLeagueButtons.clear();
	int maxnum = c->getContainer().size();
	int i = 0;
	std::vector<boost::shared_ptr<League>> leagues;
	for(auto league : c->getContainer()) {
		leagues.push_back(league.second);
	}

	std::sort(leagues.begin(), leagues.end(), [](boost::shared_ptr<League> l1, boost::shared_ptr<League> l2) {
		return l1->getLevel() < l2->getLevel(); });

	for(auto l : leagues) {
		const std::string& tname = l->getName();
		mLeagueButtons.insert(std::make_pair(tname, l));
		addSelectionButton(tname.c_str(), i, maxnum);
		i++;
	}
	mCurrentLevel = TeamBrowserLevel::Leagues;
	mCurrentCountry = c;
}

void TeamBrowser::addTeamButtons(const std::vector<boost::shared_ptr<Team>>& teams)
{
	clearCurrentButtons();
	mTeamButtons.clear();
	int maxnum = teams.size();
	int i = 0;
	for(auto team : teams) {
		const std::string& tname = team->getName();
		mTeamButtons.insert(std::make_pair(tname, team));
		addSelectionButton(tname.c_str(), i, maxnum);
		i++;
	}
	for(auto b : mCurrentButtons) {
		setTeamButtonColor(b);
	}
	mCurrentLevel = TeamBrowserLevel::Teams;
}

void TeamBrowser::addTeamButtons(boost::shared_ptr<League> l)
{
	std::vector<boost::shared_ptr<Team>> teams;
	for(auto team : l->getContainer()) {
		teams.push_back(team.second);
	}
	addTeamButtons(teams);
	mCurrentLeague = l;
}

void TeamBrowser::clearCurrentButtons()
{
	for(auto b : mCurrentButtons) {
		removeButton(b);
	}
}

void TeamBrowser::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Play") {
		clickedDone();
		return;
	} else if(buttonText == "Back") {
		switch(mCurrentLevel) {
			case TeamBrowserLevel::Continents:
			default:
				mScreenManager->dropScreen();
				break;

			case TeamBrowserLevel::Countries:
				addContinentButtons();
				break;

			case TeamBrowserLevel::Leagues:
				addCountryButtons(mCurrentContinent);
				break;

			case TeamBrowserLevel::Teams:
				addLeagueButtons(mCurrentCountry);
				break;
		}
	} else {
		switch(mCurrentLevel) {
			case TeamBrowserLevel::Continents:
				{
					auto it = mContinentButtons.find(buttonText);
					if(it != mContinentButtons.end()) {
						mCurrentContinent = it->second;
						if(enteringContinent(it->second)) {
							addCountryButtons(it->second);
						}
					}
				}
				break;

			case TeamBrowserLevel::Countries:
				{
					auto it = mCountryButtons.find(buttonText);
					if(it != mCountryButtons.end()) {
						mCurrentCountry = it->second;
						if(enteringCountry(it->second)) {
							addLeagueButtons(it->second);
						}
					}
				}
				break;

			case TeamBrowserLevel::Leagues:
				{
					auto it = mLeagueButtons.find(buttonText);
					if(it != mLeagueButtons.end()) {
						mCurrentLeague = it->second;
						if(enteringLeague(it->second)) {
							addTeamButtons(it->second);
						}
					}
				}
				break;

			case TeamBrowserLevel::Teams:
				teamClicked(button);
				break;
		}
	}
	updatePlayButton();
}

void TeamBrowser::setTeamButtonColor(boost::shared_ptr<Button> button, TeamSelection ts)
{
	switch(ts) {
		case TeamSelection::None:
			button->setColor1(Button::DefaultColor1);
			button->setColor2(Button::DefaultColor2);
			break;

		case TeamSelection::Computer:
			Menu::setButtonComputerColor(button);
			break;

		case TeamSelection::Human:
			Menu::setButtonHumanColor(button);
			break;
	}
}

void TeamBrowser::setTeamButtonColor(boost::shared_ptr<Button> button) const
{
	const std::string& buttonText = button->getText();
	auto it = mTeamButtons.find(buttonText);
	if(it != mTeamButtons.end()) {
		auto it2 = mSelectedTeams.find(it->second);
		if(it2 == mSelectedTeams.end()) {
			setTeamButtonColor(button, TeamSelection::None);
		}
		else {
			setTeamButtonColor(button, it2->second);
		}
	}
}

void TeamBrowser::teamClicked(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	auto it = mTeamButtons.find(buttonText);
	if(it != mTeamButtons.end()) {
		if(clickingOnTeam(it->second))
			setTeamButtonColor(button);
	}
}

void TeamBrowser::updatePlayButton()
{
	mPlayButton->hide();
	if(canClickDone()) {
		mPlayButton->show();
	}
}

TeamBrowserLevel TeamBrowser::getCurrentLevel() const
{
	return mCurrentLevel;
}

void TeamBrowser::setCurrentLevel(TeamBrowserLevel t)
{
	mCurrentLevel = t;
}

bool TeamBrowser::enteringLeague(boost::shared_ptr<League> p)
{
	return true;
}

bool TeamBrowser::enteringContinent(boost::shared_ptr<Continent> p)
{
	return true;
}

bool TeamBrowser::enteringCountry(boost::shared_ptr<LeagueSystem> p)
{
	return true;
}

bool TeamBrowser::clickingOnTeam(boost::shared_ptr<Team> p)
{
	auto it2 = mSelectedTeams.find(p);
	if(it2 == mSelectedTeams.end()) {
		mSelectedTeams.insert(std::make_pair(p,
					TeamSelection::Computer));
	}
	else {
		if(it2->second == TeamSelection::Computer) {
			it2->second = TeamSelection::Human;
		}
		else {
			mSelectedTeams.erase(it2);
		}
	}
	return true;
}

void TeamBrowser::toggleTeamOwnership(boost::shared_ptr<Team> p,
		std::map<boost::shared_ptr<Team>, TeamSelection>& teams)
{
	auto it2 = teams.find(p);
	if(it2 != teams.end()) {
		if(it2->second == TeamSelection::Computer) {
			it2->second = TeamSelection::Human;
		}
		else {
			it2->second = TeamSelection::Computer;
		}
	}
}

void TeamBrowser::toggleTeamOwnership(boost::shared_ptr<Team> p)
{
	toggleTeamOwnership(p, mSelectedTeams);
}

std::vector<boost::shared_ptr<StatefulTeam>> TeamBrowser::createStatefulTeams(const std::map<boost::shared_ptr<Team>, TeamSelection>& t1)
{
	std::vector<boost::shared_ptr<StatefulTeam>> teams;

	for(auto t : t1) {
		teams.push_back(boost::shared_ptr<StatefulTeam>(new StatefulTeam(*t.first,
						TeamController(t.second == TeamSelection::Human, 0),
						AITactics::createTeamTactics(*t.first))));
	}

	return teams;
}

std::vector<boost::shared_ptr<StatefulTeam>> TeamBrowser::createStatefulTeams() const
{
	return createStatefulTeams(mSelectedTeams);
}


}
