#include <iostream>
#include <memory>

#include "soccer/gui/Menu.h"

int main(int argc, char** argv)
{
	try {
		std::shared_ptr<Soccer::Menu> menu(new Soccer::Menu());
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

