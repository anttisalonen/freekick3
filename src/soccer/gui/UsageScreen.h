#ifndef SOCCER_USAGESCREEN_H
#define SOCCER_USAGESCREEN_H

#include "soccer/gui/Screen.h"

namespace Soccer {

class UsageScreen : public Screen {
	public:
		UsageScreen(boost::shared_ptr<ScreenManager> sm);
		void buttonPressed(boost::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		static const std::string ScreenName;
};

}

#endif

