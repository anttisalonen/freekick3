#ifndef SOCCER_FRIENDLYSCREEN_H
#define SOCCER_FRIENDLYSCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/Team.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/TeamBrowser.h"

namespace Soccer {

class FriendlyScreen : public TeamBrowser {
	public:
		FriendlyScreen(std::shared_ptr<ScreenManager> sm);
		bool canClickDone();
		void clickedDone();
		const std::string& getName() const;

	private:
		static const std::string ScreenName;
};

}

#endif


