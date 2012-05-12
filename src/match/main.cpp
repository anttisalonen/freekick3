#include <stdlib.h>

#include <iostream>
#include <memory>

#include "soccer/DataExchange.h"

#include "match/Match.h"
#include "match/MatchSDLGUI.h"

int main(int argc, char** argv)
{
	if(argc != 2) {
		printf("Usage: %s <path to match data file>\n", argv[0]);
		exit(1);
	}

	try {
		std::shared_ptr<Soccer::Match> matchdata = Soccer::DataExchange::parseMatchDataFile(argv[1]);
		std::shared_ptr<Match> match(new Match(*matchdata));
		std::unique_ptr<MatchSDLGUI> matchGUI(new MatchSDLGUI(match, argc, argv));
		matchGUI->play();
	}
	catch (std::exception& e) {
		printf("std::exception: %s\n", e.what());
	}
	catch(...) {
		printf("Unknown exception.\n");
	}

	return 0;
}
