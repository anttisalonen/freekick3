#ifndef SOCCER_SCREENMANAGER_H
#define SOCCER_SCREENMANAGER_H

#include <string>
#include <memory>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/Texture.h"
#include "common/Rectangle.h"

#include "soccer/DataExchange.h"

namespace Soccer {

class Screen;
class Menu;

class ScreenManager {
	public:
		ScreenManager(const Menu& m);
		~ScreenManager();
		void addScreen(std::shared_ptr<Screen> s);
		void dropScreen();
		void dropScreensUntil(const std::string& screenname);
		void drawScreen();
		bool handleEvents();
		bool isRunning() const;
		void clearScreens();
		TTF_Font* getFont();
		int getScreenWidth() const;
		int getScreenHeight() const;
		const TeamDatabase& getTeamDatabase() const;
		const PlayerDatabase& getPlayerDatabase() const;

	private:
		bool recordMouseButton(bool up, int x, int y);
		std::shared_ptr<Screen> getCurrentScreen() const;
		const Menu& mMenu;
		std::vector<std::shared_ptr<Screen>> mScreens;
		SDL_Surface* mScreen;
		TTF_Font* mFont;
		std::shared_ptr<Common::Texture> mBackground;
		std::string mPressedButton;
};

}

#endif

