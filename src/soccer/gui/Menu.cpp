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

const std::string& Menu::getDataDir()
{
	static std::string mDataDir;

	if(mDataDir.empty()) {
		const char* homedir = getenv("HOME");
		if(homedir) {
			mDataDir += homedir;
			mDataDir += "/.freekick3";
		}
	}

	return mDataDir;
}

const std::string& Menu::getSaveDir()
{
	static std::string mSaveDir;

	if(mSaveDir.empty()) {
		mSaveDir = getDataDir();
		mSaveDir += "/saves";
	}

	return mSaveDir;
}

Menu::Menu()
{
	mScreenManager.reset(new ScreenManager(*this));

	std::string datadir = getDataDir();
	datadir += "/share/teams";
	try {
		DataExchange::updatePlayerDatabase((datadir + "/Players.xml").c_str(), mPlayers);
		DataExchange::updateTeamDatabase((datadir + "/Teams.xml").c_str(), mTeams);
	} catch(std::exception& e) {
		std::cerr << "Note: could not open database files: " << e.what() << "\n";
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

	mScreenManager->addScreen(boost::shared_ptr<Screen>(new MainMenuScreen(mScreenManager)));
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

void Menu::setButtonHumanColor(boost::shared_ptr<Button> b)
{
	static Common::Color c1(128, 128, 255);
	static Common::Color c2(204, 204, 255);
	b->setColor1(c1);
	b->setColor2(c2);
}

void Menu::setButtonComputerColor(boost::shared_ptr<Button> b)
{
	static Common::Color c1(255, 128, 128);
	static Common::Color c2(255, 204, 204);
	b->setColor1(c1);
	b->setColor2(c2);
}

void Menu::setButtonSelectedColor(boost::shared_ptr<Button> b)
{
	static Common::Color c1(255, 128, 128);
	static Common::Color c2(255, 204, 204);
	b->setColor1(c1);
	b->setColor2(c2);
}

void Menu::setButtonDefaultColor(boost::shared_ptr<Button> b)
{
	b->setColor1(Button::DefaultColor1);
	b->setColor2(Button::DefaultColor2);
}

}

