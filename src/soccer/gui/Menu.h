#ifndef SOCCER_MENU_H
#define SOCCER_MENU_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include "common/Texture.h"
#include "common/Rectangle.h"

#include "soccer/DataExchange.h"

namespace Soccer {

class ScreenManager;

class Menu {
	public:
		Menu();
		void run();
		const TeamDatabase& getTeamDatabase() const;
		const PlayerDatabase& getPlayerDatabase() const;
		static const std::string& getDataDir();
		static const std::string& getSaveDir();

	private:
		boost::shared_ptr<ScreenManager> mScreenManager;
		TeamDatabase mTeams;
		PlayerDatabase mPlayers;
};

}

#endif
