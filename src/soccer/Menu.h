#ifndef SOCCER_MENU_H
#define SOCCER_MENU_H

#include <string>
#include <vector>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

#include "common/Texture.h"
#include "common/Rectangle.h"

#include "soccer/DataExchange.h"

namespace Soccer {
	class Button {
		public:
			Button(const char* text, TTF_Font* font, const Rectangle& dim);
			bool clicked(int x, int y) const;
			const std::string& getText() const;
			const Rectangle& getRectangle() const;
			const Common::Texture* getTexture() const;
		private:
			std::string mText;
			Rectangle mRectangle;
			std::shared_ptr<Common::Texture> mTextTexture;
	};

	class Menu {
		public:
			Menu();
			~Menu();
			void run();
		private:
			void drawScreen();
			bool handleEvents();
			bool recordMouseButton(bool up, int x, int y);
			SDL_Surface* mScreen;
			TTF_Font* mFont;
			std::shared_ptr<Common::Texture> mBackground;
			std::vector<std::shared_ptr<Button>> mButtons;
			bool mRunning;
			std::string mPressedButton;
			TeamDatabase mTeams;
			PlayerDatabase mPlayers;
	};
}

#endif
