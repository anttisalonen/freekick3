#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "Menu.h"
#include "common/SDL_utils.h"

namespace soccer {

using namespace Common;

Menu::Menu()
{
	mScreen = Common::initSDL();

	mFont = TTF_OpenFont("share/DejaVuSans.ttf", 12);
	if(!mFont) {
		fprintf(stderr, "Could not open font: %s\n", TTF_GetError());
		throw std::runtime_error("Loading font");
	}
	mBackground = std::shared_ptr<Texture>(new Texture("share/bg.png", 0, 0));
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

