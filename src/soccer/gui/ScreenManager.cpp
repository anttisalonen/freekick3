#include <iostream>
#include <stdexcept>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/SDL_utils.h"

#include "soccer/gui/ScreenManager.h"
#include "soccer/gui/Screen.h"
#include "soccer/gui/Menu.h"
#include "soccer/gui/Image.h"

namespace Soccer {

using namespace Common;

static const int screenWidth = 800;
static const int screenHeight = 600;

static const int fontHeight = 24;

ScreenManager::ScreenManager(const Menu& m)
	: mMenu(m),
	mPressedButton(std::string(""))
{
	mScreen = SDL_utils::initSDL(screenWidth, screenHeight, "Freekick 3");
	SDL_utils::setupOrthoScreen(screenWidth, screenHeight);

	mFont = TTF_OpenFont("share/DejaVuSans.ttf", fontHeight);
	if(!mFont) {
		fprintf(stderr, "Could not open font: %s\n", TTF_GetError());
		throw std::runtime_error("Loading font");
	}

	mBackground = boost::shared_ptr<Texture>(new Texture("share/bg.png", 0, 0));
}

ScreenManager::~ScreenManager()
{
	if(mFont)
		TTF_CloseFont(mFont);
	TTF_Quit();
	SDL_Quit();
}

void ScreenManager::addScreen(boost::shared_ptr<Screen> s)
{
	mScreens.push_back(s);
}

void ScreenManager::dropScreen()
{
	if(mScreens.size()) {
		mScreens.pop_back();
		if(!mScreens.empty()) {
			getCurrentScreen()->onReentry();
		}
	}
}

void ScreenManager::dropScreensUntil(const std::string& screenname)
{
	while(mScreens.size()) {
		if(getCurrentScreen()->getName() == screenname)
			break;
		dropScreen();
	}
}

void ScreenManager::drawScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw background
	glColor3f(0.5f, 0.5f, 0.5f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mBackground->getTexture());
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, screenHeight, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(screenWidth, screenHeight, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(screenWidth, 0.0f, 0.0f);
	glEnd();

	// draw widgets
	boost::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(currentScreen) {
		for(auto i : currentScreen->getImages()) {
			if(i->hidden())
				continue;

			i->draw(screenWidth, screenHeight);
		}

		for(auto b : currentScreen->getButtons()) {
			if(b->hidden())
				continue;

			b->draw(screenWidth, screenHeight);
		}

		for(auto b : currentScreen->getSliders()) {
			if(b->hidden())
				continue;

			b->draw(screenWidth, screenHeight);
		}
	}

	SDL_GL_SwapBuffers();
}

// return true if the screen should be redrawn.
bool ScreenManager::handleEvents()
{
	SDL_Event ev;
	int ret = SDL_WaitEvent(&ev);
	if(!ret) {
		fprintf(stderr, "Error in SDL_WaitEvent: %s\n", SDL_GetError());
		clearScreens();
		return true;
	}
	switch(ev.type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if(ev.button.button == SDL_BUTTON_LEFT)
				return recordMouseButton(ev.type == SDL_MOUSEBUTTONUP, ev.button.x, ev.button.y);
			return false;

		case SDL_MOUSEMOTION:
			return recordMouseMovement(ev.motion.state & SDL_BUTTON(1), ev.motion.x, ev.motion.y);

		case SDL_KEYDOWN:
			if(ev.key.keysym.sym == SDLK_ESCAPE) {
				dropScreen();
			} else {
				boost::shared_ptr<Screen> currentScreen = getCurrentScreen();
				if(currentScreen) {
					auto modstate = SDL_GetModState();
					auto b = currentScreen->getKeyboardShortcuts(ev.key.keysym.sym, modstate);
					if(b && b->visible())
						currentScreen->buttonPressed(b);
				}
			}
			return true;

		case SDL_QUIT:
			clearScreens();
			return true;

		case SDL_VIDEORESIZE:
		case SDL_VIDEOEXPOSE:
			return true;

		default:
			return false;
	}

	return false;
}

bool ScreenManager::recordMouseButton(bool up, int x, int y)
{
	boost::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(!currentScreen) {
		return true;
	}

	for(auto b : currentScreen->getButtons()) {
		if(!b->hidden() && b->clicked(x, y)) {
			if(!up) {
				mPressedButton = std::string(b->getText());
			} else {
				if(mPressedButton == b->getText()) {
					currentScreen->buttonPressed(b);
					mPressedButton = std::string("");
					return true;
				}
				mPressedButton = std::string("");
			}
			break;
		}
	}

	for(auto b : currentScreen->getSliders()) {
		if(!b->hidden() && b->clicked(x, y)) {
			return true;
		}
	}

	return false;
}

bool ScreenManager::recordMouseMovement(bool pressed, int x, int y)
{
	if(!pressed) {
		return false;
	}

	boost::shared_ptr<Screen> currentScreen = getCurrentScreen();
	if(!currentScreen) {
		return true;
	}

	for(auto b : currentScreen->getSliders()) {
		if(!b->hidden()) {
			if(b->clicked(x, y))
				return true;
		}
	}

	return false;
}

bool ScreenManager::isRunning() const
{
	return !mScreens.empty();
}

boost::shared_ptr<Screen> ScreenManager::getCurrentScreen() const
{
	if(mScreens.empty())
		return boost::shared_ptr<Screen>();
	else
		return mScreens[mScreens.size() - 1];
}

void ScreenManager::clearScreens()
{
	mScreens.clear();
}

TTF_Font* ScreenManager::getFont()
{
	return mFont;
}

int ScreenManager::getScreenWidth() const
{
	return screenWidth;
}

int ScreenManager::getScreenHeight() const
{
	return screenHeight;
}

const TeamDatabase& ScreenManager::getTeamDatabase() const
{
	return mMenu.getTeamDatabase();
}

const PlayerDatabase& ScreenManager::getPlayerDatabase() const
{
	return mMenu.getPlayerDatabase();
}


}
