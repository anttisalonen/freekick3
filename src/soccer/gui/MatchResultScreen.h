#ifndef SOCCER_MATCHRESULTSCREEN_H
#define SOCCER_MATCHRESULTSCREEN_H

#include <map>
#include <string>
#include <memory>

#include "soccer/gui/Screen.h"
#include "soccer/Team.h"

namespace Soccer {

class MatchResultScreen : public Screen {
	public:
		MatchResultScreen(std::shared_ptr<ScreenManager> sm, const Match& match);
		void buttonPressed(std::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		static const std::string ScreenName;
};

}

#endif



