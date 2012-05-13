#ifndef SOCCER_FRIENDLYSCREEN_H
#define SOCCER_FRIENDLYSCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/gui/Screen.h"
#include "soccer/Team.h"

namespace Soccer {

enum class TeamSelection {
	None,
	Human,
	Computer
};

class FriendlyScreen : public Screen {
	public:
		FriendlyScreen(std::shared_ptr<ScreenManager> sm);
		void buttonPressed(std::shared_ptr<Button> button);

	private:
		std::shared_ptr<Button> mPlayButton;
		std::map<std::string, std::shared_ptr<Team>> mButtonTeams;
		std::map<std::shared_ptr<Team>, TeamSelection> mSelectedTeams;
};

}

#endif


