#ifndef SOCCER_MENU_H
#define SOCCER_MENU_H

#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

#include "common/Texture.h"

namespace soccer {
	class Menu {
		public:
			Menu();
			~Menu();
			void run();
		private:
			SDL_Surface* mScreen;
			TTF_Font* mFont;
			std::shared_ptr<Common::Texture> mBackground;
	};
}

#endif
