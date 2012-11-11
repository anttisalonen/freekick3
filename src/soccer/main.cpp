#include <iostream>
#include <boost/shared_ptr.hpp>

#include "soccer/Match.h"
#include "soccer/gui/Menu.h"

void usage(const char* s)
{
	printf("Usage: %s [-h|--help] [-d|--dump <dump directory>]\n\n"
			"\t-d\t--dump\tcreate match data files for simulated matches.\n", s);
}

int main(int argc, char** argv)
{
	for(int i = 1; i < argc; i++) {
		if((!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))) {
			usage(argv[0]);
			exit(0);
		} else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dump")) {
			if(++i >= argc) { printf("-d requires an argument.\n"); exit(1); }
			printf("Setting dump directory to %s.\n", argv[i]);
			Soccer::Match::setMatchDataDumpDirectory(std::string(argv[i]));
		}
	}
	try {
		boost::shared_ptr<Soccer::Menu> menu(new Soccer::Menu());
		menu->run();
	}
	catch (std::exception& e) {
		printf("std::exception: %s\n", e.what());
	}
	catch(...) {
		printf("Unknown exception.\n");
	}

	return 0;
}

