#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

#include "soccer/gui/FriendlyScreen.h"
#include "soccer/gui/MatchResultScreen.h"

namespace Soccer {

FriendlyScreen::FriendlyScreen(std::shared_ptr<ScreenManager> sm)
	: TeamBrowser(sm)
{
}

bool FriendlyScreen::canClickDone()
{
	if(mSelectedTeams.size() == 2) {
		int numHumans = 0;
		for(auto it3 : mSelectedTeams) {
			if(it3.second == TeamSelection::Human)
				numHumans++;
		}
		return numHumans < 2;
	}
	return false;
}

void FriendlyScreen::clickedDone()
{
	int teamnum = 0;

	assert(mSelectedTeams.size() == 2);

	std::vector<std::shared_ptr<Team>> teams;
	int thisteamnum = 1;
	for(auto it : mSelectedTeams) {
		teams.push_back(it.first);
		if(it.second == TeamSelection::Human)
			teamnum = thisteamnum;
		thisteamnum++;
	}
	Match m(std::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[0], TeamController(teamnum == 1, 0), TeamTactics())),
				std::shared_ptr<StatefulTeam>(new StatefulTeam(*teams[1], TeamController(teamnum == 2, 0), TeamTactics())));

	char matchfilenamebuf[L_tmpnam];
	tmpnam(matchfilenamebuf);
	DataExchange::createMatchDataFile(m, matchfilenamebuf);
	std::cout << "Created temporary file " << matchfilenamebuf << "\n";
	playMatch(matchfilenamebuf, teamnum, 0);
	mScreenManager->addScreen(std::shared_ptr<Screen>(new MatchResultScreen(mScreenManager, matchfilenamebuf)));
	unlink(matchfilenamebuf);
	return;
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
