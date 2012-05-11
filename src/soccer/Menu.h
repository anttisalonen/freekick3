#ifndef SOCCER_MENU_H
#define SOCCER_MENU_H

#include <SDL.h>
#include <SDL_ttf.h>

namespace soccer {
	class Menu {
		public:
			Menu();
			~Menu();
			void run();
		private:
			SDL_Surface* mScreen;
	};
}

#endif
