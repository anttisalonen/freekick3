#include "Menu.h"
#include "common/SDL_utils.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

namespace soccer {

Menu::Menu()
{
	mScreen = Common::initSDL();
}

Menu::~Menu()
{
	TTF_Quit();
	SDL_Quit();
}

void Menu::run()
{
}

}

