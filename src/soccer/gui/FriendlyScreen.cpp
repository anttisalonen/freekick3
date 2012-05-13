#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/FriendlyScreen.h"

namespace Soccer {

FriendlyScreen::FriendlyScreen(std::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	float yp = 0.05f;
	for(auto t : mScreenManager->getTeamDatabase()) {
		const std::string& tname = t.second->getName();
		addButton(tname.c_str(), Common::Rectangle(0.35f, yp, 0.30f, 0.06f));
		mButtonTeams.insert(std::make_pair(tname, t.second));
		yp += 0.08f;
		if(yp > 0.90f)
			break;
	}
	addButton("Back", Common::Rectangle(0.02f, 0.90f, 0.25f, 0.06f));
	mPlayButton = addButton("Play", Common::Rectangle(0.73f, 0.90f, 0.25f, 0.06f));
	mPlayButton->hide();
}

void FriendlyScreen::buttonPressed(std::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
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
		unlink(matchfilenamebuf);
		return;
	}
	else {
		// team
		auto it = mButtonTeams.find(buttonText);
		if(it != mButtonTeams.end()) {
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


}
