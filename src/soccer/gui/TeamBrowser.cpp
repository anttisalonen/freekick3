#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/TeamBrowser.h"

namespace Soccer {

TeamBrowser::TeamBrowser(std::shared_ptr<ScreenManager> sm)
	: Screen(sm),
	mCurrentContinent(nullptr),
	mCurrentCountry(nullptr),
	mCurrentLeague(nullptr),
	mCurrentLevel(0)
{
	addButton("Back", Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	mPlayButton = addButton("Play", Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));
	mPlayButton->hide();

	addContinentButtons();
}

void TeamBrowser::addSelectionButton(const char* text, int i, int maxnum)
{
	if(i > maxnum)
		return;

	if(maxnum < 10) {
		std::shared_ptr<Button> b(addButton(text, Common::Rectangle(0.35f, 0.05f + i * 0.08, 0.25, 0.05f)));
		mCurrentButtons.push_back(b);
	}
	else {
		if(i > 30)
			return;

		std::shared_ptr<Button> b(addButton(text, Common::Rectangle(0.05f + (i % 3) * 0.30f,
						0.05f + (i / 3) * 0.08, 0.25, 0.05f)));
		mCurrentButtons.push_back(b);
	}
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
	mCurrentLevel = 0;
}

void TeamBrowser::addCountryButtons(std::shared_ptr<Continent> c)
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
	mCurrentLevel = 1;
	mCurrentContinent = c;
}

void TeamBrowser::addLeagueButtons(std::shared_ptr<LeagueSystem> c)
{
	clearCurrentButtons();
	mLeagueButtons.clear();
	int maxnum = c->getContainer().size();
	int i = 0;
	for(auto league : c->getContainer()) {
		const std::string& tname = league.second->getName();
		mLeagueButtons.insert(std::make_pair(tname, league.second));
		addSelectionButton(tname.c_str(), i, maxnum);
		i++;
	}
	mCurrentLevel = 2;
	mCurrentCountry = c;
}

void TeamBrowser::addTeamButtons(std::shared_ptr<League> l)
{
	clearCurrentButtons();
	mTeamButtons.clear();
	int maxnum = l->getContainer().size();
	int i = 0;
	for(auto team : l->getContainer()) {
		const std::string& tname = team.second->getName();
		mTeamButtons.insert(std::make_pair(tname, team.second));
		addSelectionButton(tname.c_str(), i, maxnum);
		i++;
	}
	for(auto b : mCurrentButtons) {
		setTeamButtonColor(b);
	}

	mCurrentLevel = 3;
	mCurrentLeague = l;
}

void TeamBrowser::clearCurrentButtons()
{
	for(auto b : mCurrentButtons) {
		removeButton(b);
	}
}

void TeamBrowser::buttonPressed(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		switch(mCurrentLevel) {
			case 0:
			default:
				mScreenManager->dropScreen();
				return;

			case 1:
				addContinentButtons();
				return;

			case 2:
				addCountryButtons(mCurrentContinent);
				return;

			case 3:
				addLeagueButtons(mCurrentCountry);
				return;
		}
	}
	else if(buttonText == "Play") {
		clickedDone();
		return;
	}
	else {
		switch(mCurrentLevel) {
			case 0:
				{
					// continent
					auto it = mContinentButtons.find(buttonText);
					if(it != mContinentButtons.end()) {
						if(enteringContinent(it->second)) {
							addCountryButtons(it->second);
						}
					}
				}
				break;

			case 1:
				{
					// country
					auto it = mCountryButtons.find(buttonText);
					if(it != mCountryButtons.end()) {
						if(enteringCountry(it->second)) {
							addLeagueButtons(it->second);
						}
					}
				}
				break;

			case 2:
				{
					// league
					auto it = mLeagueButtons.find(buttonText);
					if(it != mLeagueButtons.end()) {
						if(enteringLeague(it->second)) {
							addTeamButtons(it->second);
						}
					}
				}
				break;

			case 3:
				// team
				teamClicked(button);
				break;

		}
	}
}

void TeamBrowser::setTeamButtonColor(std::shared_ptr<Button> button) const
{
	const std::string& buttonText = button->getText();
	auto it = mTeamButtons.find(buttonText);
	if(it != mTeamButtons.end()) {
		auto it2 = mSelectedTeams.find(it->second);
		if(it2 == mSelectedTeams.end()) {
			button->setColor1(Button::DefaultColor1);
			button->setColor2(Button::DefaultColor2);
		}
		else {
			if(it2->second == TeamSelection::Computer) {
				button->setColor1(Common::Color(255, 128, 128));
				button->setColor2(Common::Color(255, 204, 204));
			}
			else {
				button->setColor1(Common::Color(128, 128, 255));
				button->setColor2(Common::Color(204, 204, 255));
			}
		}
	}
}

void TeamBrowser::teamClicked(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	auto it = mTeamButtons.find(buttonText);
	if(it != mTeamButtons.end()) {
		auto it2 = mSelectedTeams.find(it->second);
		if(it2 == mSelectedTeams.end()) {
			mSelectedTeams.insert(std::make_pair(it->second,
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
		setTeamButtonColor(button);
	}

	mPlayButton->hide();
	if(canClickDone()) {
		mPlayButton->show();
	}
}

int TeamBrowser::getCurrentLevel() const
{
	return mCurrentLevel;
}

bool TeamBrowser::enteringLeague(std::shared_ptr<League> p)
{
	return true;
}

bool TeamBrowser::enteringContinent(std::shared_ptr<Continent> p)
{
	return true;
}

bool TeamBrowser::enteringCountry(std::shared_ptr<LeagueSystem> p)
{
	return true;
}


}
