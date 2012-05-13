#ifndef SOCCER_FRIENDLYSCREEN_H
#define SOCCER_FRIENDLYSCREEN_H

#include "soccer/gui/Screen.h"

namespace Soccer {

class FriendlyScreen : public Screen {
	public:
		FriendlyScreen(std::shared_ptr<ScreenManager> sm);
		void buttonPressed(const std::string& buttonText);
};

}

#endif


