#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include "soccer/Match.h"

namespace Soccer {

Match::Match(const std::shared_ptr<StatefulTeam> t1, const std::shared_ptr<StatefulTeam> t2)
	: mTeam1(t1),
	mTeam2(t2)
{
}

MatchResult Match::play(bool display) const
{
	if(display) {
		char matchfilenamebuf[L_tmpnam];
		tmpnam(matchfilenamebuf);
		DataExchange::createMatchDataFile(*this, matchfilenamebuf);
		std::cout << "Created temporary file " << matchfilenamebuf << "\n";
		int teamnum = 0;
		if(mTeam1->getController().HumanControlled && !mTeam2->getController().HumanControlled)
			teamnum = 1;
		else if(!mTeam1->getController().HumanControlled && mTeam2->getController().HumanControlled)
			teamnum = 2;
		playMatch(matchfilenamebuf, teamnum, 0);
		std::shared_ptr<Match> match = DataExchange::parseMatchDataFile(matchfilenamebuf);
		unlink(matchfilenamebuf);
		return match->getResult();
	}
	else {
		/* TODO: simulate better */
		int h = rand() % 4;
		int a = rand() % 4;
		return MatchResult(h, a);
	}
}

void Match::playMatch(const char* datafile, int teamnum, int playernum)
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

const MatchResult& Match::getResult() const
{
	return mResult;
}

void Match::setResult(const MatchResult& m)
{
	mResult = m;
}

const std::shared_ptr<StatefulTeam> Match::getTeam(int i) const
{
	if(i == 0)
		return mTeam1;
	else
		return mTeam2;
}

}


