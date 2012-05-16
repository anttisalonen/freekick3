#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/FriendlyScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

FriendlyScreen::FriendlyScreen(std::shared_ptr<ScreenManager> sm)
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

void FriendlyScreen::addSelectionButton(const char* text, int i, int maxnum)
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

void FriendlyScreen::addContinentButtons()
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

void FriendlyScreen::addCountryButtons(std::shared_ptr<Continent> c)
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

void FriendlyScreen::addLeagueButtons(std::shared_ptr<LeagueSystem> c)
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

void FriendlyScreen::addTeamButtons(std::shared_ptr<League> l)
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
	mCurrentLevel = 3;
	mCurrentLeague = l;
}

void FriendlyScreen::clearCurrentButtons()
{
	for(auto b : mCurrentButtons) {
		removeButton(b);
	}
}

void FriendlyScreen::buttonPressed(std::shared_ptr<Button> button)
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

			case 4:
				addTeamButtons(mCurrentLeague);
				return;
		}
	}
	else if(buttonText == "Play") {
		int teamnum = 0;
		if(mSelectedTeams.size() != 2)
			return;
		std::vector<std::shared_ptr<Team>> teams;
		int thisteamnum = 1;
		for(auto it : mSelectedTeams) {
			teams.push_back(it.first);
			if(it.second == TeamSelection::Human)
				teamnum = thisteamnum;
			thisteamnum++;
		}
		Match m(teams[0], teams[1], TeamTactics(), TeamTactics());

		char matchfilenamebuf[L_tmpnam];
		tmpnam(matchfilenamebuf);
		DataExchange::createMatchDataFile(m, matchfilenamebuf);
		std::cout << "Created temporary file " << matchfilenamebuf << "\n";
		playMatch(matchfilenamebuf, teamnum, 0);
		mScreenManager->addScreen(std::shared_ptr<Screen>(new MatchResultScreen(mScreenManager, matchfilenamebuf)));
		unlink(matchfilenamebuf);
		return;
	}
	else {
		switch(mCurrentLevel) {
			case 0:
				{
					// continent
					auto it = mContinentButtons.find(buttonText);
					if(it != mContinentButtons.end()) {
						addCountryButtons(it->second);
					}
				}
				break;

			case 1:
				{
					// country
					auto it = mCountryButtons.find(buttonText);
					if(it != mCountryButtons.end()) {
						addLeagueButtons(it->second);
					}
				}
				break;

			case 2:
				{
					// league
					auto it = mLeagueButtons.find(buttonText);
					if(it != mLeagueButtons.end()) {
						addTeamButtons(it->second);
					}
				}
				break;

			case 3:
				// team
				{
					clickedOnTeam(button);
				}
				break;

		}
	}
}

void FriendlyScreen::clickedOnTeam(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	auto it = mTeamButtons.find(buttonText);
	if(it != mTeamButtons.end()) {
		auto it2 = mSelectedTeams.find(it->second);
		if(it2 == mSelectedTeams.end()) {
			mSelectedTeams.insert(std::make_pair(it->second,
						TeamSelection::Human));
			button->setColor1(Common::Color(128, 128, 255));
			button->setColor2(Common::Color(204, 204, 255));
		}
		else {
			if(it2->second == TeamSelection::Human) {
				it2->second = TeamSelection::Computer;
				button->setColor1(Common::Color(255, 128, 128));
				button->setColor2(Common::Color(255, 204, 204));
			}
			else {
				mSelectedTeams.erase(it2);
				button->setColor1(Button::DefaultColor1);
				button->setColor2(Button::DefaultColor2);
			}
		}
	}

	mPlayButton->hide();
	if(mSelectedTeams.size() == 2) {
		int numHumans = 0;
		for(auto it3 : mSelectedTeams) {
			if(it3.second == TeamSelection::Human)
				numHumans++;
		}
		if(numHumans < 2)
			mPlayButton->show();
	}
}

void FriendlyScreen::playMatch(const char* datafile, int teamnum, int playernum)
{
	pid_t fret = fork();
	if(fret == 0) {
		/* child */
		std::vector<const char*> args;
		args.push_back("freekick3-match");
		args.push_back(datafile);
		if(teamnum == 0) {
			args.push_back("-o");
		}
		else {
			args.push_back("-t");
			args.push_back(std::to_string(teamnum).c_str());
			if(playernum != 0) {
				args.push_back("-p");
				args.push_back(std::to_string(playernum).c_str());
			}
		}

		std::cout << "Running command: ";
		for(auto arg : args) {
			std::cout << arg << " ";
		}
		std::cout << "\n";

		args.push_back((char*)0);

		char* const* argsarray = const_cast<char* const*>(&args[0]);

		if(execvp("freekick3-match", argsarray) == -1) {
			/* try bin/freekick3-match */
			char cwdbuf[256];
			if(getcwd(cwdbuf, 256) == NULL) {
				perror("getcwd");
				exit(1);
			}
			else {
				std::string fullpath(cwdbuf);
				fullpath += "/bin/freekick3-match";
				if(execv(fullpath.c_str(), argsarray) == -1) {
					perror("execl");
					fprintf(stderr, "tried running %s\n", fullpath.c_str());
					exit(1);
				}
			}
		}
	}
	else if(fret != -1) {
		/* parent */
		while(1) {
			pid_t waited = wait(NULL);
			if(waited == fret)
				break;
		}
	}
	else {
		perror("fork");
	}
}

const std::string FriendlyScreen::ScreenName = std::string("Friendly Screen");

const std::string& FriendlyScreen::getName() const
{
	return ScreenName;
}


}
