#include <stdlib.h>

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "soccer/DataExchange.h"

#include "match/Match.h"
#include "match/MatchSDLGUI.h"

void usage(const char* p)
{
	printf("Usage: %s <path to match data file> [-o] [-t team] [-p player] [-f FPS] [-d]\n\n"
			"\t-o\tobserver mode\n"
			"\t-t team\tteam number (1 or 2)\n"
			"\t-p num\tplayer number (1-11)\n"
			"\t-f FPS\tfixed frame rate\n"
			"\t-d\tdebug mode\n\n",
			p);
}

int main(int argc, char** argv)
{
	if(argc < 2 || (argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		usage(argv[0]);
		exit(1);
	}

	bool observer = false;
	bool debug = false;
	int teamnum = 1;
	int playernum = 0;
	int ticksPerSec = 0;

	for(int i = 2; i < argc; i++) {
		if(!strcmp(argv[i], "-o")) {
			observer = true;
		}
		else if(!strcmp(argv[i], "-d")) {
			debug = true;
		}
		else if(!strcmp(argv[i], "-p")) {
			i++;
			if(i >= argc) {
				printf("-p requires a numeric argument between 1 and 11.\n");
				exit(1);
			}
			int num = atoi(argv[i]);
			if(num < 1 || num > 11) {
				printf("-p requires a numeric argument between 1 and 11.\n");
				exit(1);
			}
			playernum = num;
		}
		else if(!strcmp(argv[i], "-t")) {
			i++;
			if(i >= argc) {
				printf("-t requires a numeric argument between 1 and 2.\n");
				exit(1);
			}
			int num = atoi(argv[i]);
			if(num < 1 || num > 2) {
				printf("-t requires a numeric argument between 1 and 2.\n");
				exit(1);
			}
			teamnum = num;
		}
		else if(!strcmp(argv[i], "-f")) {
			i++;
			if(i >= argc) {
				printf("-f requires a numeric argument.\n");
				exit(1);
			}
			ticksPerSec = atoi(argv[i]);
		}
	}
	try {
		boost::shared_ptr<Soccer::Match> matchdata = Soccer::DataExchange::parseMatchDataFile(argv[1]);
		boost::shared_ptr<Match> match(new Match(*matchdata));
		std::unique_ptr<MatchSDLGUI> matchGUI(new MatchSDLGUI(match, observer, teamnum, playernum,
					ticksPerSec, debug));
		if(matchGUI->play()) {
			// finished match
			printf("Final score: %d - %d\n", match->getResult().HomeGoals,
					match->getResult().AwayGoals);
			Soccer::DataExchange::createMatchDataFile(*match, argv[1]);
		}
	}
	catch (std::exception& e) {
		printf("std::exception: %s\n", e.what());
	}
	catch(...) {
		printf("Unknown exception.\n");
	}

	return 0;
}
