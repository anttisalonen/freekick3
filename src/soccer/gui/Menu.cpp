#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "Menu.h"
#include "common/SDL_utils.h"

#include "soccer/DataExchange.h"
#include "soccer/Player.h"
#include "soccer/Match.h"

#include "soccer/gui/MainMenuScreen.h"
#include "soccer/gui/ScreenManager.h"

namespace Soccer {

using namespace Common;

Menu::Menu()
{
	mScreenManager.reset(new ScreenManager(*this));

	std::string datadir;
	const char* homedir = getenv("HOME");
	if(homedir) {
		datadir += homedir;
		datadir += "/.freekick3/share/teams";
		try {
			DataExchange::updatePlayerDatabase((datadir + "/Players.xml").c_str(), mPlayers);
			DataExchange::updateTeamDatabase((datadir + "/Teams.xml").c_str(), mTeams);
		} catch(std::exception& e) {
			std::cerr << "Note: could not open database files: " << e.what() << "\n";
		}
	}

	if(mPlayers.empty())
		DataExchange::updatePlayerDatabase("share/teams/Players.xml", mPlayers);
	if(mTeams.getContainer().empty())
		DataExchange::updateTeamDatabase("share/teams/Teams.xml", mTeams);

	for(auto c : mTeams.getContainer())
		for(auto lsys : c.second->getContainer())
			for(auto league : lsys.second->getContainer())
				for(auto t : league.second->getContainer())
					t.second->fetchPlayersFromDB(mPlayers);

	mScreenManager->addScreen(std::shared_ptr<Screen>(new MainMenuScreen(mScreenManager)));
}

void Menu::run()
{
	srand(time(NULL));
	mScreenManager->drawScreen();
	while(mScreenManager->isRunning()) {
		if(mScreenManager->handleEvents())
			mScreenManager->drawScreen();
	}
}

const TeamDatabase& Menu::getTeamDatabase() const
{
	return mTeams;
}

const PlayerDatabase& Menu::getPlayerDatabase() const
{
	return mPlayers;
}

}

