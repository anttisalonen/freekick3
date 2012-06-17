#ifndef SOCCER_LOADGAMESCREEN_H
#define SOCCER_LOADGAMESCREEN_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/gui/Screen.h"

namespace Soccer {

class LoadGameScreen : public Screen {
	public:
		LoadGameScreen(boost::shared_ptr<ScreenManager> sm);
		void buttonPressed(boost::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		void collectSavedFiles();
		std::vector<std::string> mSavedFiles;
		static const std::string ScreenName;
};

}

#endif


