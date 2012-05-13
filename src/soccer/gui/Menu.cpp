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
	DataExchange::updatePlayerDatabase("share/teams/Players_1000.xml", mPlayers);
	DataExchange::updateTeamDatabase("share/teams/Teams.xml", mTeams);
	for(auto t : mTeams) {
		t.second->fetchPlayersFromDB(mPlayers);
	}

	mScreenManager->addScreen(std::shared_ptr<Screen>(new MainMenuScreen(mScreenManager)));
}

void Menu::run()
{
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

