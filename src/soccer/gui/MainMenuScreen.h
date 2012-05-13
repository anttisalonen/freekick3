#ifndef SOCCER_MAINMENUSCREEN_H
#define SOCCER_MAINMENUSCREEN_H

#include "soccer/gui/Screen.h"

namespace Soccer {

class MainMenuScreen : public Screen {
	public:
		MainMenuScreen(std::shared_ptr<ScreenManager> sm);
		void buttonPressed(std::shared_ptr<Button> button);
};

}

#endif

