#include <iostream>
#include <memory>

#include "Match.h"
#include "MatchSDLGUI.h"

int main(int argc, char** argv)
{
	try {
		std::shared_ptr<Match> match(new Match());
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
