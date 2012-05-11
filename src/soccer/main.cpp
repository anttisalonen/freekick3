#include <iostream>
#include <memory>

#include "soccer/Menu.h"

int main(int argc, char** argv)
{
	try {
		std::shared_ptr<soccer::Menu> menu(new soccer::Menu());
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

